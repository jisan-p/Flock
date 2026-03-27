#ifndef LOGIN_SCREEN_HPP
#define LOGIN_SCREEN_HPP

#include "Screen.hpp"
#include "../network/TCPClient.hpp"
#include <string>

class LoginScreen : public Screen {
private:
    std::string nextScreen;
    TCPClient& client;  // Reference to the shared network client

    std::string username;
    std::string password;
    std::string errorMsg;

public:
    LoginScreen(TCPClient& client);
    void render() override;
    void handleInput() override;
    std::string getNextScreen() override;
};

#endif
