#include "ui/LoadingScreen.hpp"
#include "ui/MenuScreen.hpp"
#include "ui/Terminal.hpp"
#include <iostream>

int main() {
    // 1. Show the loading screen first
    LoadingScreen splash;
    splash.render();
    
    // Clean up terminal a bit after the splash screen
    Terminal::clear();
    Terminal::moveCursor(1, 1);
    
    std::string next = splash.getNextScreen();
    
    // 2. Simple ScreenManager Loop
    if (next == "MenuScreen") {
        MenuScreen menu;
        while (true) {
            menu.render();
            menu.handleInput();
            
            std::string menuNext = menu.getNextScreen();
            if (menuNext != "" && menuNext != "MenuScreen") {
                // If it wants to go to LoginScreen or RegisterScreen, 
                // we would handle it here. For now, since we only have MenuScreen
                // fully implemented that we're testing, we will exit if it leaves.
                Terminal::clear();
                std::cout << "Transitioning to: " << menuNext << "\n";
                break;
            }
        }
    }
    
    Terminal::clear();
    std::cout << "Exited gracefully.\n";
    return 0;
}