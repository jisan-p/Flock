#ifndef LOADING_SCREEN_HPP
#define LOADING_SCREEN_HPP

#include "Screen.hpp" // The abstract base class we discussed
#include <string>

class LoadingScreen : public Screen {
private:
    std::string nextScreen;
    bool isConnected;

public:
    LoadingScreen();
    void render() override;
    void handleInput() override;
    std::string getNextScreen() override;
};

#endif