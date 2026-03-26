#pragma once

#include "Screen.hpp" // The abstract base class we discussed
#include <string>

class LoadingScreen : public Screen {
private:
    std::string nextScreen;
    bool isConnected;

public:
    LoadingScreen(); // why constructor is here? because we need to initialize the nextScreen and isConnected
    void render() override;
    void handleInput() override;
    std::string getNextScreen() override;
};