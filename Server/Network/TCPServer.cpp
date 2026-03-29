#include "TCPServer.hpp"
#include "Protocol.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ============================================================
// Constructor & Destructor
// ============================================================

TCPServer::TCPServer(Database& database, int port)
    : db(database), port(port), serverSocket(-1), running(false) {}

TCPServer::~TCPServer() {
    stop();
}

// ============================================================
// start() - Binds, listens, and accepts clients in a loop
// ============================================================

void TCPServer::start() {
    // 1. Create the socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error: Could not create server socket.\n";
        return;
    }

    // Allow port reuse (avoids "address already in use" error)
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind to the port
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    serverAddr.sin_port        = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error: Could not bind to port " << port << ".\n";
        close(serverSocket);
        return;
    }

    // 3. Start listening (queue up to 10 pending connections)
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Error: Could not listen on socket.\n";
        close(serverSocket);
        return;
    }

    running = true;
    std::cout << "=== Flock Server started on port " << port << " ===\n";

    // 4. Accept loop - spawns a thread for each new client
    while (running) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        if (clientSocket < 0) {
            if (running) std::cerr << "Warning: Failed to accept a client.\n";
            continue;
        }

        // Get the client's IP for logging
        std::string clientIP = inet_ntoa(clientAddr.sin_addr);
        std::cout << "[+] New client connected from " << clientIP << "\n";

        // Spawn a thread to handle this client
        clientThreads.emplace_back(&TCPServer::handleClient, this, clientSocket);
    }
}

// ============================================================
// stop() - Shuts down the server
// ============================================================

void TCPServer::stop() {
    running = false;
    if (serverSocket >= 0) {
        close(serverSocket);
        serverSocket = -1;
    }

    // Wait for all client threads to finish
    for (auto& t : clientThreads) {
        if (t.joinable()) t.join();
    }
    clientThreads.clear();
}

// ============================================================
// handleClient() - Runs in its own thread, one per client
// ============================================================

void TCPServer::handleClient(int clientSocket) {
    std::string currentUser = "";  // Empty until they login

    while (running) {
        std::string line = readLine(clientSocket);

        // If readLine returns empty, the client disconnected
        if (line.empty()) {
            break;
        }

        // Process the command and get back the username (if login happened)
        std::string result = processCommand(line, clientSocket, currentUser);
        if (!result.empty()) {
            currentUser = result;
        }
    }

    // ---- Cleanup: remove from online clients ----
    if (!currentUser.empty()) {
        std::lock_guard<std::mutex> lock(clientsMutex);
        onlineClients.erase(currentUser);
        std::cout << "[-] User '" << currentUser << "' disconnected.\n";
    }

    close(clientSocket);
}

// ============================================================
// processCommand() - Parses and handles one protocol command
// ============================================================

std::string TCPServer::processCommand(const std::string& command, int clientSocket, const std::string& currentUser) {
    std::vector<std::string> parts = Protocol::split(command);

    if (parts.empty()) return "";

    std::string cmd = parts[0];

    // ---- LOGIN|username|password ----
    if (cmd == Protocol::LOGIN && parts.size() >= 3) {
        std::string username = parts[1];
        std::string password = parts[2];

        try {
            if (db.verifyLogin(username, password)) {
                // Get their userID
                std::string userID = db.getUserID(username);

                // Add to online clients
                {
                    std::lock_guard<std::mutex> lock(clientsMutex);
                    onlineClients[username] = clientSocket;
                }

                sendToSocket(clientSocket, Protocol::join({Protocol::LOGIN_OK, userID, username}));
                std::cout << "[*] User '" << username << "' logged in.\n";
                return username;  // Return username to set currentUser
            } else {
                sendToSocket(clientSocket, Protocol::join({Protocol::LOGIN_FAIL, "Invalid credentials"}));
            }
        } catch (const Database::readingError& e) {
            sendToSocket(clientSocket, Protocol::join({Protocol::LOGIN_FAIL, e.what()}));
        }
    }

    // ---- REGISTER|username|password ----
    else if (cmd == Protocol::REGISTER && parts.size() >= 3) {
        std::string username = parts[1];
        std::string password = parts[2];

        // Generate a simple userID from the username
        std::string userID = "U" + std::to_string(std::hash<std::string>{}(username) % 10000);

        User newUser(userID, username, password);

        try {
            // Check if username already exists
            if (!db.getUserID(username).empty()) {
                sendToSocket(clientSocket, Protocol::join({Protocol::REGISTER_FAIL, "Username already exists"}));
            } else {
                db.registerUser(newUser);

                // Auto-login after register
                {
                    std::lock_guard<std::mutex> lock(clientsMutex);
                    onlineClients[username] = clientSocket;
                }

                sendToSocket(clientSocket, Protocol::join({Protocol::REGISTER_OK, userID, username}));
                std::cout << "[*] New user registered: '" << username << "'\n";
                return username;
            }
        } catch (const Database::writingError& e) {
            sendToSocket(clientSocket, Protocol::join({Protocol::REGISTER_FAIL, e.what()}));
        }
    }

    // ---- SEND|targetUser|message ----
    else if (cmd == Protocol::SEND && parts.size() >= 3 && !currentUser.empty()) {
        std::string targetUser = parts[1];
        std::string message = parts[2];

        try {
            db.appendMessage(currentUser, targetUser, currentUser, message);
            sendToSocket(clientSocket, Protocol::join({Protocol::SEND_OK}));

            // Push the message to the target user if they are online
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                auto it = onlineClients.find(targetUser);
                if (it != onlineClients.end()) {
                    // Send: MSG|senderUsername|message
                    sendToSocket(it->second, Protocol::join({Protocol::MSG, currentUser, message}));
                }
            }
        } catch (const Database::writingError& e) {
            sendToSocket(clientSocket, Protocol::join({Protocol::SEND_FAIL, e.what()}));
        }
    }

    // ---- HISTORY|targetUser ----
    else if (cmd == Protocol::HISTORY && parts.size() >= 2 && !currentUser.empty()) {
        std::string targetUser = parts[1];

        std::vector<std::string> history = db.loadChatHistory(currentUser, targetUser);

        if (history.empty()) {
            sendToSocket(clientSocket, Protocol::join({Protocol::HISTORY_EMPTY}));
        } else {
            // Join all history lines with a special separator (;; between lines)
            std::string combined;
            for (size_t i = 0; i < history.size(); i++) {
                if (i > 0) combined += ";;";
                combined += history[i];
            }
            sendToSocket(clientSocket, Protocol::join({Protocol::HISTORY_DATA, combined}));
        }
    }

    // ---- LIST_USERS ----
    else if (cmd == Protocol::LIST_USERS && !currentUser.empty()) {
        std::vector<std::string> users = db.listUsers();

        std::string userList;
        for (size_t i = 0; i < users.size(); i++) {
            if (i > 0) userList += ",";
            userList += users[i];
        }
        sendToSocket(clientSocket, Protocol::join({Protocol::USERS_LIST, userList}));
    }

    // ---- LOGOUT ----
    else if (cmd == Protocol::LOGOUT) {
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            onlineClients.erase(currentUser);
        }
        std::cout << "[-] User '" << currentUser << "' logged out.\n";
        return "";  // Clear currentUser
    }

    return "";  // No change to currentUser
}

// ============================================================
// sendToSocket() - Sends a string message over the socket
// ============================================================

void TCPServer::sendToSocket(int socket, const std::string& message) {
    send(socket, message.c_str(), message.size(), 0);
}

// ============================================================
// readLine() - Reads one line (up to \n) from the socket
// ============================================================

std::string TCPServer::readLine(int socket) {
    std::string line;
    char ch;

    while (true) {
        int bytesRead = recv(socket, &ch, 1, 0);

        if (bytesRead <= 0) {
            return "";  // Client disconnected or error
        }

        if (ch == '\n') {
            break;  // End of line
        }

        line += ch;
    }

    return line;
}
