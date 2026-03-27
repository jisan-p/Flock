#pragma once

#include "User.hpp"
#include <string>
#include <mutex>
#include <vector>

class Database {
private:
    // File paths for simulated database storage
    std::string usersFile;
    std::string chatDirectory;

    // Mutexes to prevent thread collisions during file I/O
    std::mutex dbMutex;    // Protects users.txt
    std::mutex chatMutex;  // Protects files in chats_1-1/

    // Helper function to generate consistent 1-to-1 chat filenames
    std::string getChatFilename(const std::string& user1, const std::string& user2) const;

public:
    // Custom exception for read operations
    class readingError {
        std::string msg;
    public:
        readingError(std::string msg) : msg(msg) {}
        std::string what() const { 
            return msg; 
        }
    };

    // Custom exception for write operations
    class writingError {
        std::string msg;
    public:
        writingError(std::string msg) : msg(msg) {}
        std::string what() const { 
            return msg; 
        }
    };

    // Constructor to initialize paths
    Database();

    // User Authentication & Management
    bool registerUser(const User& newUser);
    bool verifyLogin(const std::string& targetUsername, const std::string& targetPassword);


    // 1-to-1 Chat Operations

    
    // Appends a new timestamped message to the userA_userB.txt file
    bool appendMessage(const std::string& user1, const std::string& user2, const std::string& senderID, const std::string& encryptedMsg);
    
    // Reads the chat history from the userA_userB.txt file
    // Returns a vector of strings, where each string is a formatted line from the file
    std::vector<std::string> loadChatHistory(const std::string& user1, const std::string& user2);


    // User Lookup

    
    // Returns a list of all registered usernames
    std::vector<std::string> listUsers();
    
    // Returns the userID for a given username, or empty string if not found
    std::string getUserID(const std::string& username);
};