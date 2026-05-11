// Flock Client - main.cpp
// Shows screens in order: Loading -> Menu -> Login/Register -> Home -> ChatRoom

#include "ui/LoadingScreen.hpp"
#include "ui/MenuScreen.hpp"
#include "ui/LoginScreen.hpp"
#include "ui/RegisterScreen.hpp"
#include "ui/HomeScreen.hpp"
#include "ui/ChatRoomScreen.hpp"
#include "ui/Terminal.hpp"
#include "network/TCPClient.hpp"
#include <iostream>

int main() {
    TCPClient client("127.0.0.1", 4444);

    LoadingScreen splash;
    splash.render();

    bool connected = client.connectToServer();

    Terminal::clear();
    Terminal::moveCursor(1, 1);

    if (!connected) {
        std::cout << "Could not connect to server at 127.0.0.1:4444\n";
        std::cout << "Make sure the Flock server is running first!\n";
        return 1;
    }

    std::string currentScreen = "MenuScreen";

    while (true) {
        //main menu
        if (currentScreen == "MenuScreen") {
            MenuScreen menu;
            while (true) {
                menu.render();
                menu.handleInput();

                std::string next = menu.getNextScreen();
                if (!next.empty() && next != "MenuScreen") {
                    currentScreen = next;
                    break;
                }
            }
        }

        //login screen
        else if (currentScreen == "LoginScreen") {
            LoginScreen login(client);
            login.render();
            login.handleInput();

            std::string next = login.getNextScreen();
            if (next == "HomeScreen") {
                currentScreen = "HomeScreen";
            } else {
                currentScreen = "MenuScreen";
            }
        }

        //register screen
        else if (currentScreen == "RegisterScreen") {
            RegisterScreen reg(client);
            reg.render();
            reg.handleInput();

            std::string next = reg.getNextScreen();
            if (next == "HomeScreen") {
                currentScreen = "HomeScreen";
            } else {
                currentScreen = "MenuScreen";
            }
        }

        //home screen
        else if (currentScreen == "HomeScreen") {
            HomeScreen home(client);
            home.render();
            home.handleInput();

            std::string next = home.getNextScreen();
            if (next == "ChatRoomScreen") {
                // Open the chat room with the selected user
                std::string partner = home.getSelectedUser();

                ChatRoomScreen chat(client, partner);
                chat.render();
                chat.handleInput();

                // After chat, go back to home  
                currentScreen = "HomeScreen";
            } else {
                currentScreen = "MenuScreen";
            }
        }

        //unknown screen this is for 
        else {
            Terminal::clear();
            std::cout << "Unknown screen: " << currentScreen << "\n";
            break;
        }
    }

    Terminal::clear();
    Terminal::showCursor();
    std::cout << "Flock closed. Goodbye!\n";
    return 0;
}
