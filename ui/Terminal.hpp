#ifndef TERMINAL_HPP // Include guard
#define TERMINAL_HPP

#include <iostream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace Terminal {

// Captures a single raw keystroke without waiting for Enter
inline int getch() {
#ifdef _WIN32
  return _getch();
#else
  struct termios oldt, newt;
  int ch;
  // 1. Get current terminal settings
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // 2. Disable canonical mode and local echo
  newt.c_lflag &= ~(ICANON | ECHO);

  // 3. Apply new settings, read char, and restore old settings
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return ch;
#endif
}

// Clears the entire terminal screen and moves cursor to top-left (1,1).
// Uses cursor-home THEN clear so it never causes scrolling on macOS Terminal.
inline void clear() { std::cout << "\033[H\033[2J"; }

// Enters the "alternate screen buffer" – a completely separate surface.
// Nothing drawn here touches the user's normal scrollback history.
// Call this once at program start (before any other drawing).
inline void enterAlternateScreen() { std::cout << "\033[?1049h" << std::flush; }

// Exits the alternate screen buffer and restores the user's normal terminal.
// Call this once at program exit (before printing any goodbye message).
inline void exitAlternateScreen() { std::cout << "\033[?1049l" << std::flush; }

// Teleports the cursor to a specific row (Y) and column (X)
// Note: Terminal coordinates are 1-indexed (top-left is 1,1)
inline void moveCursor(int row, int col) {
  std::cout << "\033[" << row << ";" << col << "H";
}

// A helper function to jump to a coordinate and immediately print text
inline void printAt(int row, int col, const std::string &text) {
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
} // namespace Terminal

#endif // TERMINAL_HPP