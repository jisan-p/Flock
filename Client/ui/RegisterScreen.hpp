#ifndef REGISTER_SCREEN_HPP
#define REGISTER_SCREEN_HPP

#include "Screen.hpp"
#include "../network/TCPClient.hpp"
#include <string>

class RegisterScreen : public Screen {
private:
    std::string nextScreen;
    TCPClient& client;  // Reference to the shared network client

    std::string username;
    std::string password;
    std::string confirmPassword;
    std::string errorMsg;

public:
    RegisterScreen(TCPClient& client);
    void render() override;
    void handleInput() override;
    std::string getNextScreen() override;
};

#endif
