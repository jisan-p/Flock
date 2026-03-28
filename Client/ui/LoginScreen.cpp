#include "LoginScreen.hpp"
#include "Terminal.hpp"
#include "color.hpp"
#include <mutex>

LoginScreen::LoginScreen(TCPClient& client)
    : client(client), nextScreen(""), username(""), password(""), errorMsg("") {}

void LoginScreen::render() {
    std::lock_guard<std::mutex> lock(Terminal::screenMutex);

    Terminal::clear();
    Terminal::hideCursor();

    // Draw the login form
    Terminal::printAt(3, 30, Color::bold(Color::cyan("=== Flock Login ===")));
    Terminal::printAt(6, 15, "Username: ");
    Terminal::printAt(8, 15, "Password: ");

    // Show typed username and password (masked)
    Terminal::printAt(6, 25, username);
    std::string masked(password.size(), '*');
    Terminal::printAt(8, 25, masked);

    Terminal::printAt(11, 15, Color::yellow("[Enter your credentials and press Enter]"));

    // Show error message if any
    if (!errorMsg.empty()) {
        Terminal::printAt(14, 15, Color::red(errorMsg));
    }

    Terminal::printAt(17, 15, "Press [ESC] to go back to menu");

    Terminal::showCursor();
    Terminal::flush();
}

void LoginScreen::handleInput() {
    // Reset
    username = "";
    password = "";
    errorMsg = "";

    // First render
    render();

    // ---- Read username ----
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::moveCursor(6, 25);
        Terminal::showCursor();
        Terminal::flush();
    }

    // Read username character by character
    while (true) {
        int ch = Terminal::getch();

        if (ch == 27) {  // ESC - go back
            nextScreen = "MenuScreen";
            return;
        }
        if (ch == 10 || ch == 13) {  // Enter - move to password
            break;
        }
        if (ch == 127 || ch == 8) {  // Backspace
            if (!username.empty()) {
                username.pop_back();
                std::lock_guard<std::mutex> lock(Terminal::screenMutex);
                Terminal::printAt(6, 25, username + " ");
                Terminal::moveCursor(6, 25 + (int)username.size());
                Terminal::flush();
            }
            continue;
        }
        if (ch >= 32 && ch <= 126) {  // Printable character
            username += (char)ch;
            std::lock_guard<std::mutex> lock(Terminal::screenMutex);
            Terminal::printAt(6, 25, username);
            Terminal::flush();
        }
    }

    // Read password 
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::moveCursor(8, 25);
        Terminal::flush();
    }

    while (true) {
        int ch = Terminal::getch();

        if (ch == 27) {  // ESC - go back
            nextScreen = "MenuScreen";
            return;
        }
        if (ch == 10 || ch == 13) {  // Enter - submit
            break;
        }
        if (ch == 127 || ch == 8) {  // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::lock_guard<std::mutex> lock(Terminal::screenMutex);
                std::string masked(password.size(), '*');
                Terminal::printAt(8, 25, masked + " ");
                Terminal::moveCursor(8, 25 + (int)password.size());
                Terminal::flush();
            }
            continue;
        }
        if (ch >= 32 && ch <= 126) {  // Printable character
            password += (char)ch;
            std::lock_guard<std::mutex> lock(Terminal::screenMutex);
            std::string masked(password.size(), '*');
            Terminal::printAt(8, 25, masked);
            Terminal::flush();
        }
    }

    // Try to login
    if (username.empty() || password.empty()) {
        errorMsg = "Username and password cannot be empty!";
        render();
        Terminal::getch();  // Wait for a key
        return;
    }

    // Show loading message
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::printAt(14, 15, Color::yellow("Logging in...        "));
        Terminal::flush();
    }

    if (client.login(username, password)) {
        nextScreen = "HomeScreen";
    } else {
        errorMsg = "Login failed! Wrong username or password.";
        render();
        Terminal::getch();  // Wait for a key before going back
    }
}

std::string LoginScreen::getNextScreen() {
    return nextScreen;
}
