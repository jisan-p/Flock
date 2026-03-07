#include "Terminal.hpp"

mutex ConsoleUtils::consoleMutex;

void ConsoleUtils::setCursor(int row, int col) {
  cout << getCursorCode(row, col);
}

string ConsoleUtils::getCursorCode(int row, int col) {
  return "\033[" + to_string(row) + ";" + to_string(col) + "H";
}

string ConsoleUtils::getClearLineCode() { 
    return "\033[2K"; 
}