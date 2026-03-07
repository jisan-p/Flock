#ifndef CONSOLE_UTILS_HPP
#define CONSOLE_UTILS_HPP

#include <iostream>
#include <mutex>
#include <string>

using namespace std;

class ConsoleUtils {
public:
  // Global mutex for synchronizing console output across threads
  static mutex consoleMutex;

  // Move cursor to specific row and column (Direct Output)
  static void setCursor(int row, int col);

  // Get ANSI code to move cursor
  static string getCursorCode(int row, int col);

  // Get ANSI code to clear line
  static string getClearLineCode();
};

#endif
