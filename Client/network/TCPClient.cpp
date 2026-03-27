#include "TCPClient.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

//constructor & destructor
TCPClient::TCPClient(const std::string& ip, int port)
    : serverIP(ip), serverPort(port), sock(-1), connected(false), listening(false) {}

TCPClient::~TCPClient() {
    stopListening();
    disconnect();
}

//connectToServer() - Establishes TCP connection

bool TCPClient::connectToServer() {
    // 1. Create the socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error: Could not create socket.\n";
        return false;
    }

    // 2. Set up the server address
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(serverPort);

    // Convert IP string to binary
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid server IP address.\n";
        close(sock);
        sock = -1;
        return false;
    }

    // 3. Connect to the server
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(sock);
        sock = -1;
        return false;
    }

    connected = true;
    return true;
}


//disconnect() - Closes the connection

void TCPClient::disconnect() {
    connected = false;
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}

bool TCPClient::isConnected() const {
    return connected;
}


//sendCommand():  sends a command n reads one line response

std::string TCPClient::sendCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(sendMutex);

    //send the command (add newline if not present)
    std::string msg = command;
    if (msg.empty() || msg.back() != '\n') {
        msg += '\n';
    }

    if (send(sock, msg.c_str(), msg.size(), 0) < 0) {
        return "";
    }


    //reading the response line
    std::string response;
    char ch;
    while (true) {
        int bytesRead = recv(sock, &ch, 1, 0);
        if (bytesRead <= 0) 
        {
            connected = false;
            return "";
        }
        if (ch == '\n') break;
        response += ch;
    }

    return response;
}


// High-level commands:


bool TCPClient::login(const std::string& username, const std::string& password) {
    std::string response = sendCommand("LOGIN|" + username + "|" + password);

    // Parse: LOGIN_OK|userID|username  or  LOGIN_FAIL|reason
    std::stringstream ss(response);
    std::string status;
    std::getline(ss, status, '|');

    if (status == "LOGIN_OK") {
        std::getline(ss, myUserID, '|');
        std::getline(ss, myUsername, '|');
        return true;
    }
    return false;
}

bool TCPClient::registerUser(const std::string& username, const std::string& password) {
    std::string response = sendCommand("REGISTER|" + username + "|" + password);

    std::stringstream ss(response);
    std::string status;
    std::getline(ss, status, '|');

    if (status == "REGISTER_OK") {
        std::getline(ss, myUserID, '|');
        std::getline(ss, myUsername, '|');
        return true;
    }
    return false;
}

std::vector<std::string> TCPClient::listUsers() {
    std::string response = sendCommand("LIST_USERS");

    // Parse: USERS_LIST|user1,user2,user3
    std::vector<std::string> users;
    std::stringstream ss(response);
    std::string status, userList;

    std::getline(ss, status, '|');
    std::getline(ss, userList);

    if (status == "USERS_LIST" && !userList.empty()) {
        std::stringstream userStream(userList);
        std::string user;
        while (std::getline(userStream, user, ',')) {
            if (!user.empty()) {
                users.push_back(user);
            }
        }
    }
    return users;
}

bool TCPClient::sendMessage(const std::string& targetUser, const std::string& message) {
    // If the listener is running, just fire-and-forget (don't block on recv)
    // The listener thread is already reading from the socket
    if (listening) {
        sendOnly("SEND|" + targetUser + "|" + message);
        return true;
    }
    // Otherwise, use normal send + recv
    std::string response = sendCommand("SEND|" + targetUser + "|" + message);
    return (response.find("SEND_OK") != std::string::npos);
}

void TCPClient::sendOnly(const std::string& command) {
    std::lock_guard<std::mutex> lock(sendMutex);
    
    std::string msg = command + "\n";
    send(sock, msg.c_str(), msg.size(), 0);
}

std::vector<std::string> TCPClient::getChatHistory(const std::string& targetUser) {
    std::string response = sendCommand("HISTORY|" + targetUser);

    std::vector<std::string> history;
    std::stringstream ss(response);
    std::string status, data;

    std::getline(ss, status, '|');
    std::getline(ss, data);

    if (status == "HISTORY_DATA" && !data.empty()) {
        // Lines are separated by ";;"
        size_t pos = 0;
        size_t found;
        while ((found = data.find(";;", pos)) != std::string::npos) {
            history.push_back(data.substr(pos, found - pos));
            pos = found + 2;
        }
        // Last line
        if (pos < data.size()) {
            history.push_back(data.substr(pos));
        }
    }
    return history;
}

// Real-time message listener

void TCPClient::setMessageCallback(std::function<void(const std::string&)> callback) {
    messageCallback = callback;
}

void TCPClient::startListening() {
    listening = true;
    listenerThread = std::thread(&TCPClient::listenerLoop, this);
}

void TCPClient::stopListening() {
    listening = false;
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
}

void TCPClient::listenerLoop() {
    while (listening && connected) {
        // Use select() with a timeout so we can check the listening flag
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);

        struct timeval timeout;
        timeout.tv_sec  = 0;
        timeout.tv_usec = 200000;  // 200ms timeout

        int result = select(sock + 1, &readSet, nullptr, nullptr, &timeout);

        if (result > 0 && FD_ISSET(sock, &readSet)) {
            // Data available - read one line
            std::string line;
            char ch;

            while (true) {
                int bytesRead = recv(sock, &ch, 1, MSG_PEEK);
                if (bytesRead <= 0) {
                    connected = false;
                    return;
                }

                // Actually consume the byte
                recv(sock, &ch, 1, 0);

                if (ch == '\n') break;
                line += ch;
            }

            // Only forward MSG lines to the callback (ignore SEND_OK etc.)
            if (!line.empty() && messageCallback && line.rfind("MSG|", 0) == 0) {
                messageCallback(line);
            }
        }
    }
}
