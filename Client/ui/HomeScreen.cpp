#include "HomeScreen.hpp"
#include "Terminal.hpp"
#include "color.hpp"
#include <mutex>

HomeScreen::HomeScreen(TCPClient& client)
    : client(client), nextScreen(""), selectedIndex(0), selectedUser("") {}

void HomeScreen::render() {
    std::lock_guard<std::mutex> lock(Terminal::screenMutex);

    Terminal::clear();
    Terminal::hideCursor();

    // Header
    Terminal::printAt(2, 5, Color::bold(Color::cyan("Flock - Home")));
    Terminal::printAt(2, 50, Color::green("Logged in as: " + client.myUsername));

    Terminal::printAt(4, 5, "Select a user to chat with:");
    Terminal::printAt(4, 45, Color::yellow("[Arrow keys + Enter]"));

    // User list
    if (userList.empty()) {
        Terminal::printAt(7, 10, Color::yellow("No other users found. Ask a friend to register!"));
    } else {
        for (int i = 0; i < (int)userList.size(); i++) {
            int row = 7 + i;
            std::string label = "  " + userList[i];

            if (i == selectedIndex) {
                label = "> " + userList[i];
                Terminal::printAt(row, 10, Color::cyan(label));
            } else {
                Terminal::printAt(row, 10, label);
            }
        }
    }

    // Footer
    int footerRow = 7 + (int)userList.size() + 2;
    Terminal::printAt(footerRow, 5, "[R] Refresh list    [ESC] Logout & back to menu");

    Terminal::showCursor();
    Terminal::flush();
}

void HomeScreen::handleInput() {
    // Fetch the user list from the server
    userList = client.listUsers();

    // Remove our own username from the list
    std::vector<std::string> filtered;
    for (const auto& u : userList) {
        if (u != client.myUsername) {
            filtered.push_back(u);
        }
    }
    userList = filtered;
    selectedIndex = 0;

    // Render and handle input loop
    while (true) {
        render();

        int key = Terminal::getch();

        if (key == 27) {
            int next1 = Terminal::getch();
            if (next1 == '[') {
                int next2 = Terminal::getch();
                if (next2 == 'A') {  // Up arrow
                    if (selectedIndex > 0) selectedIndex--;
                } else if (next2 == 'B') {  // Down arrow
                    if (selectedIndex < (int)userList.size() - 1) selectedIndex++;
                }
            } else {
                client.sendCommand("LOGOUT");
                nextScreen = "MenuScreen";
                return;
            }
        }

        else if (key == 'r' || key == 'R') {
            userList = client.listUsers();
            std::vector<std::string> filtered2;
            for (const auto& u : userList) {
                if (u != client.myUsername) {
                    filtered2.push_back(u);
                }
            }
            userList = filtered2;
            selectedIndex = 0;
        }

        else if ((key == 10 || key == 13) && !userList.empty()) {
            // Enter - select user to chat with
            selectedUser = userList[selectedIndex];
            nextScreen = "ChatRoomScreen";
            return;
        }
    }
}

std::string HomeScreen::getNextScreen() {
    return nextScreen;
}

std::string HomeScreen::getSelectedUser() const {
    return selectedUser;
}
