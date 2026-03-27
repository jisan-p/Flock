#ifndef CHAT_ROOM_SCREEN_HPP
#define CHAT_ROOM_SCREEN_HPP

#include "Screen.hpp"
#include "../network/TCPClient.hpp"
#include <string>
#include <vector>
#include <mutex>

class ChatRoomScreen : public Screen {
private:
    std::string nextScreen;
    TCPClient& client;
    std::string chatPartner;  // The user we're chatting with

    std::vector<std::string> messages;  // Displayed messages
    std::mutex messagesMutex;           // Protects the messages vector
    std::string inputBuffer;            // Current input text

public:
    ChatRoomScreen(TCPClient& client, const std::string& partner);
    void render() override;
    void handleInput() override;
    std::string getNextScreen() override;

    // Adds a message to the display (called by the listener callback)
    void addMessage(const std::string& msg);
};

#endif
