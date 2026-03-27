#include "Database.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

// constructor initializes the paths
Database::Database() {
    usersFile = "../Database/users.txt";
    chatDirectory = "../Database/chats_1-1/";
}


// user Authentication & Management
bool Database::registerUser(const User& newUser) {
    std::lock_guard<std::mutex> lock(dbMutex); // lock the database for this thread's operation
    
    std::ofstream file(usersFile, std::ios::app);
    if (!file.is_open()) {
        throw writingError("Critical: users.txt opening for registration failed.");
    }

    file << newUser.serialize();
    file.close();
    return true;
}

bool Database::verifyLogin(const std::string& targetUsername, const std::string& targetPassword) {
    std::lock_guard<std::mutex>  lock(dbMutex); 

    std::ifstream file(usersFile, std::ios::in);
    if (!file.is_open()) {
        //if the file doesn't exist yet, no users registered — login fails
        return false;
    }

    std::string line;
    User tempUser;
    
    // Read the file line by line to find a matching user
    while (std::getline(file, line)) {
        if (User::deserialize(line, tempUser)) {
            if (tempUser.username == targetUsername && tempUser.password == targetPassword) {
                file.close();
                return true; 
            }
        }
    }
    
    file.close();
    return false; 
}


// 1-to-1 Chat Operations:


// generation of consistent filenames
std::string Database::getChatFilename(const std::string& user1, const std::string& user2) const {
    // sort the user IDs alphabetically to ensure consistency
    std::string first = (user1 < user2) ? user1 : user2;
    std::string second = (user1 < user2) ? user2 : user1;
    
    return chatDirectory + first + "_" + second + ".txt";
}

bool Database::appendMessage(const std::string& user1, const std::string& user2, const std::string& senderID, const std::string& encryptedMsg) {
    // using the separate chat mutex to avoid blocking logins
    std::lock_guard<std::mutex> lock(chatMutex); 

    std::string filename = getChatFilename(user1, user2);
    std::ofstream file(filename, std::ios::app);

    if (!file.is_open()) {
        throw writingError("Failed to open chat file for appending: " + filename);
    }

    // generation of a simple Unix timestamp
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    // append the formatted data: timestamp|senderID|message
    file << timestamp << "|" << senderID << "|" << encryptedMsg << "\n";
    
    file.close();
    return true;
}

std::vector<std::string> Database::loadChatHistory(const std::string& user1, const std::string& user2) {
    std::lock_guard<std::mutex> lock(chatMutex);
    
    std::string filename = getChatFilename(user1, user2);
    std::ifstream file(filename, std::ios::in);
    
    std::vector<std::string> history;

    // if the file doesn't exist yet (no messages sent), return an empty history safely
    if (!file.is_open()) {
        return history; 
    }

    std::string line;
    while (std::getline(file, line)) {
        history.push_back(line);
    }

    file.close();
    return history;
}


// User Lookup
std::vector<std::string> Database::listUsers() {
    std::lock_guard<std::mutex> lock(dbMutex);

    std::vector<std::string> users;
    std::ifstream file(usersFile, std::ios::in);

    if (!file.is_open()) {
        return users;  // No users file yet, return empty list
    }

    std::string line;
    User tempUser;
    while (std::getline(file, line)) {
        if (User::deserialize(line, tempUser)) {
            users.push_back(tempUser.username);
        }
    }

    file.close();
    return users;
}

std::string Database::getUserID(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);

    std::ifstream file(usersFile, std::ios::in);
    if (!file.is_open()) {
        return "";  // No users file yet
    }

    std::string line;
    User tempUser;
    while (std::getline(file, line)) {
        if (User::deserialize(line, tempUser)) {
            if (tempUser.username == username) {
                file.close();
                return tempUser.userID;
            }
        }
    }



    file.close();
    return "";  //user not found
}