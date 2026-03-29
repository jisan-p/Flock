#pragma once

// Protocol.hpp - Shared text protocol for Flock
// Messages are pipe-delimited strings ending with newline (\n)
// Example: "LOGIN|username|password\n"

#include <string>
#include <vector>
#include <sstream>

namespace Protocol {

    // Command strings (Client -> Server)
    const std::string LOGIN       = "LOGIN";
    const std::string REGISTER    = "REGISTER";
    const std::string SEND        = "SEND";
    const std::string HISTORY     = "HISTORY";
    const std::string LIST_USERS  = "LIST_USERS";
    const std::string LOGOUT      = "LOGOUT";

    // Response strings (Server -> Client)
    const std::string LOGIN_OK    = "LOGIN_OK";
    const std::string LOGIN_FAIL  = "LOGIN_FAIL";
    const std::string REGISTER_OK = "REGISTER_OK";
    const std::string REGISTER_FAIL = "REGISTER_FAIL";
    const std::string SEND_OK     = "SEND_OK";
    const std::string SEND_FAIL   = "SEND_FAIL";
    const std::string HISTORY_DATA = "HISTORY_DATA";
    const std::string HISTORY_EMPTY = "HISTORY_EMPTY";
    const std::string USERS_LIST  = "USERS_LIST";
    const std::string MSG         = "MSG";  // Server pushes incoming message to client

    // Helper: split a string by '|' delimiter
    inline std::vector<std::string> split(const std::string& line) {
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string token;

        while (std::getline(ss, token, '|')) {
            parts.push_back(token);
        }
        return parts;
    }

    // Helper: join parts with '|' delimiter
    inline std::string join(const std::vector<std::string>& parts) {
        std::string result;
        for (size_t i = 0; i < parts.size(); i++) {
            if (i > 0) result += "|";
            result += parts[i];
        }
        return result + "\n";
    }
}
