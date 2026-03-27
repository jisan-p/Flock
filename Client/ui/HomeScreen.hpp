#pragma once

#include "Screen.hpp"
#include "../network/TCPClient.hpp" // Lets talk about it later
#include <string>
#include <vector>

// Inherited from Screen
class HomeScreen : public Screen {
private:
    std::string nextScreen;
    TCPClient& client;

    std::vector<std::string> userList;   // List of registered users
    int selectedIndex;                   // Currently highlighted user
    std::string selectedUser;            // The user chosen to chat with

public:
    HomeScreen(TCPClient& client);
    void render() override;
    void handleInput() override;
    std::string getNextScreen() override;

    // Getter for ChatRoomScreen to know who we're chatting with
    std::string getSelectedUser() const;
};
