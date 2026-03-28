#pragma once

#include "User.hpp"
#include <string>
#include <mutex>
#include <vector>

class Database {
private:
    std::string usersFile;
    std::string chatDirectory;

    std::mutex dbMutex;    
    std::mutex chatMutex;  

    std::string getChatFilename(const std::string& user1, const std::string& user2) const;

public:
    class readingError {
        std::string msg;
    public:
        readingError(std::string msg) : msg(msg) {}
        std::string what() const { 
            return msg; 
        }
    };

    class writingError {
        std::string msg;
    public:
        writingError(std::string msg) : msg(msg) {}
        std::string what() const { 
            return msg; 
        }
    };

    Database();

    // user authentication & management:

    bool registerUser(const User& newUser);
    bool verifyLogin(const std::string& targetUsername, const std::string& targetPassword);


    // 1-to-1 Chat Operations:
    
    bool appendMessage(const std::string& user1, const std::string& user2, const std::string& senderID, const std::string& encryptedMsg);
    
    std::vector<std::string> loadChatHistory(const std::string& user1, const std::string& user2);
    
    // user lookup:

    std::vector<std::string> listUsers();
    std::string getUserID(const std::string& username);
};