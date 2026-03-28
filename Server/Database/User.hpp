#pragma once


#include <string>
#include <sstream>

class User {
public:
    std::string userID;
    std::string username;
    std::string password;

    User(std::string id = "", std::string name = "", std::string pass = "")
        : userID(id), username(name), password(pass) {}

    // Serialize for saving to users.txt
    // Format: userID|username|password
    std::string serialize() const {
        std::stringstream ss;
        ss << userID << "|" << username << "|" << password << "\n";
        return ss.str();
    }

    // Deserialize a line from users.txt
    static bool deserialize(const std::string& line, User& obj) {
        std::stringstream ss(line);

        if (!std::getline(ss, obj.userID, '|'))   return false;
        if (!std::getline(ss, obj.username, '|')) return false;

        // Read the rest of the line as password (handles with or without trailing pipe)
        if (!std::getline(ss, obj.password))       return false;

        // Remove trailing pipe if present
        if (!obj.password.empty() && obj.password.back() == '|') {
            obj.password.pop_back();
        }

        return true;
    }
};