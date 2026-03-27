#pragma once

// ============================================================
// TCPServer.hpp - Multi-threaded TCP server for Flock
// ============================================================
// Listens on a port, spawns a thread per client, and routes
// protocol commands to the Database class.
// ============================================================

#include <string>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include "../Database/Database.hpp"

class TCPServer {
private:
    int serverSocket;                   // The listening socket
    int port;                           // Port to listen on
    Database& db;                       // Reference to the shared database
    std::atomic<bool> running;          // Flag to stop the server

    // ---- Connected clients tracking ----
    // Maps username -> their socket fd (for pushing real-time messages)
    std::map<std::string, int> onlineClients;
    std::mutex clientsMutex;  // Protects the onlineClients map

    // ---- Thread management ----
    std::vector<std::thread> clientThreads;

    // ---- Per-client handler (runs in its own thread) ----
    void handleClient(int clientSocket);

    // ---- Process a single protocol command from a client ----
    // Returns the username if logged in, empty string otherwise
    std::string processCommand(const std::string& command, int clientSocket, const std::string& currentUser);

    // ---- Helper: send a string to a socket ----
    void sendToSocket(int socket, const std::string& message);

    // ---- Helper: read a line from a socket ----
    std::string readLine(int socket);

public:
    // Constructor takes a Database reference and port number
    TCPServer(Database& database, int port = 4444);

    // Destructor cleans up sockets
    ~TCPServer();

    // Start listening and accepting clients (blocking call)
    void start();

    // Stop the server
    void stop();
};
