#include "ChatRoomScreen.hpp"
#include "Terminal.hpp"
#include "color.hpp"
#include <sstream>

ChatRoomScreen::ChatRoomScreen(TCPClient& client, const std::string& partner)
    : client(client), chatPartner(partner), nextScreen(""), inputBuffer("") {}

void ChatRoomScreen::render() {
    std::lock_guard<std::mutex> lock(Terminal::screenMutex);

    Terminal::clear();
    Terminal::hideCursor();

    // ---- Header ----
    Terminal::printAt(1, 5, Color::bold(Color::cyan("Chat with " + chatPartner)));
    Terminal::printAt(1, 55, Color::green("You: " + client.myUsername));
    Terminal::printAt(2, 5, std::string(70, '-'));

    // ---- Messages area (rows 3 to 20) ----
    {
        std::lock_guard<std::mutex> msgLock(messagesMutex);

        int maxDisplay = 16;  // Number of message lines to show
        int startIdx = 0;
        if ((int)messages.size() > maxDisplay) {
            startIdx = (int)messages.size() - maxDisplay;
        }

        int row = 3;
        for (int i = startIdx; i < (int)messages.size(); i++) {
            // Parse the message: format is "sender: message"
            std::string line = messages[i];

            // Color our own messages differently
            if (line.find(client.myUsername + ": ") == 0) {
                Terminal::printAt(row, 5, Color::green(line));
            } else {
                Terminal::printAt(row, 5, Color::white(line));
            }
            row++;
        }
    }

    // ---- Input area ----
    Terminal::printAt(20, 5, std::string(70, '-'));
    Terminal::printAt(21, 5, Color::yellow("> ") + inputBuffer);
    Terminal::printAt(23, 5, "[Enter] Send    [ESC] Back to Home");

    Terminal::showCursor();
    Terminal::moveCursor(21, 7 + (int)inputBuffer.size());
    Terminal::flush();
}

void ChatRoomScreen::handleInput() {
    // Load chat history from the server
    std::vector<std::string> history = client.getChatHistory(chatPartner);

    {
        std::lock_guard<std::mutex> lock(messagesMutex);
        messages.clear();

        // Parse history lines: timestamp|sender|message
        for (const auto& line : history) {
            std::stringstream ss(line);
            std::string timestamp, sender, msg;
            std::getline(ss, timestamp, '|');
            std::getline(ss, sender, '|');
            std::getline(ss, msg);

            messages.push_back(sender + ": " + msg);
        }
    }

    // Set up the real-time message callback
    client.setMessageCallback([this](const std::string& rawMsg) {
        // Parse: MSG|sender|content
        std::stringstream ss(rawMsg);
        std::string cmd, sender, content;
        std::getline(ss, cmd, '|');
        std::getline(ss, sender, '|');
        std::getline(ss, content);

        if (cmd == "MSG" && sender == chatPartner) {
            addMessage(sender + ": " + content);
            render();  // Refresh the screen
        }
    });

    // Start listening for incoming messages
    client.startListening();

    // Main input loop
    inputBuffer = "";
    render();

    while (true) {
        int ch = Terminal::getch();

        if (ch == 27) {
            // Check if it's an arrow key (ESC + '[' + letter) or just ESC
            // For simplicity, treat any ESC as "go back"
            client.stopListening();
            client.setMessageCallback(nullptr);
            nextScreen = "HomeScreen";
            return;
        }

        if (ch == 10 || ch == 13) {
            // Enter - send the message
            if (!inputBuffer.empty()) {
                // Send to server
                client.sendMessage(chatPartner, inputBuffer);

                // Add to our display
                addMessage(client.myUsername + ": " + inputBuffer);

                inputBuffer = "";
                render();
            }
        }

        else if (ch == 127 || ch == 8) {
            // Backspace
            if (!inputBuffer.empty()) {
                inputBuffer.pop_back();
                render();
            }
        }

        else if (ch >= 32 && ch <= 126) {
            // Printable character
            inputBuffer += (char)ch;

            // Update just the input line for speed
            std::lock_guard<std::mutex> lock(Terminal::screenMutex);
            Terminal::printAt(21, 5, Color::yellow("> ") + inputBuffer + "  ");
            Terminal::moveCursor(21, 7 + (int)inputBuffer.size());
            Terminal::flush();
        }
    }
}

std::string ChatRoomScreen::getNextScreen() {
    return nextScreen;
}

void ChatRoomScreen::addMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(messagesMutex);
    messages.push_back(msg);
}
