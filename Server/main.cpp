// Flock Server - main.cpp
// will create a Database and TCPServer, then starts listening.

#include "Database/Database.hpp"
#include "Network/TCPServer.hpp"
#include <iostream>

int main() {
    std::cout << "   Flock Server v0.1\n";
    std::cout << "\n";
    
    // 1. create shared database (file-based)
    Database db;
    
    // 2. create the tcp server on port no 4444
    TCPServer server(db, 4444);
    
    // 3. start server
    std::cout << "Starting server...\n";
    server.start();
    
    std::cout << "   Logs:\n";
    std::cout << "\n";
    return 0;
}
