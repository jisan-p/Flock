#include "RegisterScreen.hpp"
#include "Terminal.hpp"
#include "color.hpp"
#include <mutex>

RegisterScreen::RegisterScreen(TCPClient& client)
    : client(client), nextScreen(""), username(""), password(""), confirmPassword(""), errorMsg("") {}

void RegisterScreen::render() {
    std::lock_guard<std::mutex> lock(Terminal::screenMutex);

    Terminal::clear();
    Terminal::hideCursor();

    Terminal::printAt(3, 27, Color::bold(Color::cyan("=== Create Account ===")));
    Terminal::printAt(6, 15, "Username:         ");
    Terminal::printAt(8, 15, "Password:         ");
    Terminal::printAt(10, 15, "Confirm Password: ");

    // Show typed values
    Terminal::printAt(6, 33, username);
    std::string masked1(password.size(), '*');
    Terminal::printAt(8, 33, masked1);
    std::string masked2(confirmPassword.size(), '*');
    Terminal::printAt(10, 33, masked2);

    Terminal::printAt(13, 15, Color::yellow("[Fill in the fields and press Enter]"));

    if (!errorMsg.empty()) {
        Terminal::printAt(16, 15, Color::red(errorMsg));
    }

    Terminal::printAt(19, 15, "Press [ESC] to go back to menu");

    Terminal::showCursor();
    Terminal::flush();
}

// Helper function to read a text field with character-by-character input
static std::string readField(int row, int col, bool masked, bool& escaped) {
    std::string value;
    escaped = false;

    while (true) {
        int ch = Terminal::getch();

        if (ch == 27) {  // ESC
            escaped = true;
            return "";
        }
        if (ch == 10 || ch == 13) {  // Enter
            break;
        }
        if (ch == 127 || ch == 8) {  // Backspace
            if (!value.empty()) {
                value.pop_back();
                std::lock_guard<std::mutex> lock(Terminal::screenMutex);
                std::string display = masked ? std::string(value.size(), '*') : value;
                Terminal::printAt(row, col, display + " ");
                Terminal::moveCursor(row, col + (int)value.size());
                Terminal::flush();
            }
            continue;
        }
        if (ch >= 32 && ch <= 126) {  // Printable
            value += (char)ch;
            std::lock_guard<std::mutex> lock(Terminal::screenMutex);
            std::string display = masked ? std::string(value.size(), '*') : value;
            Terminal::printAt(row, col, display);
            Terminal::flush();
        }
    }

    return value;
}

void RegisterScreen::handleInput() {
    username = "";
    password = "";
    confirmPassword = "";
    errorMsg = "";
    render();

    bool escaped = false;

    // Read username
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::moveCursor(6, 33);
        Terminal::showCursor();
        Terminal::flush();
    }
    username = readField(6, 33, false, escaped);
    if (escaped) { nextScreen = "MenuScreen"; return; }

    // Read password
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::moveCursor(8, 33);
        Terminal::flush();
    }
    password = readField(8, 33, true, escaped);
    if (escaped) { nextScreen = "MenuScreen"; return; }

    // Read confirm password
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::moveCursor(10, 33);
        Terminal::flush();
    }
    confirmPassword = readField(10, 33, true, escaped);
    if (escaped) { nextScreen = "MenuScreen"; return; }

    // ---- Validate ----
    if (username.empty() || password.empty()) {
        errorMsg = "Username and password cannot be empty!";
        render();
        Terminal::getch();
        return;
    }

    if (password != confirmPassword) {
        errorMsg = "Passwords do not match!";
        render();
        Terminal::getch();
        return;
    }

    // Show loading
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::printAt(16, 15, Color::yellow("Creating account...      "));
        Terminal::flush();
    }

    if (client.registerUser(username, password)) {
        nextScreen = "HomeScreen";
    } else {
        errorMsg = "Registration failed! Username may already exist.";
        render();
        Terminal::getch();
    }
}

std::string RegisterScreen::getNextScreen() {
    return nextScreen;
}
