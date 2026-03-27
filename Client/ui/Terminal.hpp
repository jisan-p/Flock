#ifndef TERMINAL_HPP
#define TERMINAL_HPP

// uses ANSI Escape Codes to treat the terminal like a 2D canvas

#include <iostream>
#include <string>
#include <mutex>

/////
#include <termios.h> 
#include <unistd.h>
#include <cstdio>

namespace Terminal {
    extern std::mutex screenMutex; // extern is used to declare a variable that is defined in another file
    
    inline int getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = std::getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    inline void exitAlternateScreen() {
        std::cout << "\033[?1049l";
    }
    



    // Clears the entire terminal screen and moves cursor to top-left (1,1)
    inline void clear() {   std::cout << "\033[2J\033[H"; }

    inline void moveCursor(int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H";
    }

    inline void printAt(int row, int col, const std::string& text) {
        moveCursor(row, col);
        std::cout << text;
    }

    inline void hideCursor() {  std::cout << "\033[?25l"; }

    inline void showCursor() {  std::cout << "\033[?25h"; }
    
    inline void flush() { std::cout << std::flush; }
}

#endif // TERMINAL_HPP