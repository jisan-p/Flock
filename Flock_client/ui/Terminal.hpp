#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <iostream>
#include <string>
#include <mutex>

namespace Terminal {
    inline std::mutex screenMutex;


    // Clears the entire terminal screen and moves cursor to top-left (1,1)
    inline void clear() {
        std::cout << "\033[2J\033[H";
    }

    // Teleports the cursor to a specific row (Y) and column (X)
    // Note: Terminal coordinates are 1-indexed (top-left is 1,1)
    inline void moveCursor(int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H";
    }

    // A helper function to jump to a coordinate and immediately print text
    inline void printAt(int row, int col, const std::string& text) {
        moveCursor(row, col);
        std::cout << text;
    }

    // Hides the blinking cursor (prevents flickering during UI redraws)
    inline void hideCursor() {
        std::cout << "\033[?25l";
    }

    // Shows the blinking cursor again (use this when waiting for user input)
    inline void showCursor() {
        std::cout << "\033[?25h";
    }
    
    // Optional: Flushes the output buffer immediately to the screen
    inline void flush() {
        std::cout << std::flush;
    }
}

#endif // TERMINAL_HPP