#pragma once

#include <string>
#include <sstream>
#include <vector>

class Room {
public:
    std::string roomID;
    std::string roomName;
    std::vector<std::string> memberIDs;

    Room(std::string id = "", std::string name = "")
        : roomID(id), roomName(name) {}

    // Serialize for saving to rooms.txt
    std::string serialize() const {
        std::stringstream ss;
        ss << roomID << "|" << roomName << "|";

        for (size_t i = 0; i < memberIDs.size(); i++) {
            if (i > 0) ss << ",";
            ss << memberIDs[i];
        }
        ss << "\n";
        return ss.str();
    }

    // Deserialize a line from rooms.txt
    static bool deserialize(const std::string& line, Room& obj) {
        std::stringstream ss(line);
        std::string membersStr;

        if (!std::getline(ss, obj.roomID, '|'))   return false;
        if (!std::getline(ss, obj.roomName, '|')) return false;
        if (!std::getline(ss, membersStr))        return false;

        // Split the members by comma
        obj.memberIDs.clear();
        std::stringstream memberStream(membersStr);
        std::string member;
        while (std::getline(memberStream, member, ',')) {
            if (!member.empty()) {
                obj.memberIDs.push_back(member);
            }
        }

        return true;
    }
};
