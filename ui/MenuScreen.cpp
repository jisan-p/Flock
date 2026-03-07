#include "MenuScreen.hpp"
#include "Terminal.hpp"
#include <iostream>

MenuScreen::MenuScreen() {
  nextScreen = "";
  selectedIndex = 1; // Default to the first option
}

void MenuScreen::render() {
  Terminal::clear();
  Terminal::hideCursor();

  // Draw the static skeleton
  Terminal::printAt(5, 38, "Flock");

  // Draw the options. Highlight the selected one.
  Terminal::printAt(10, 10,
                    (selectedIndex == 1 ? "> [1]  Login" : "  [1]  Login"));
  Terminal::printAt(
      12, 10,
      (selectedIndex == 2 ? "> [2]  Create Account" : "  [2]  Create Account"));
  Terminal::printAt(
      14, 10, (selectedIndex == 3 ? "> [3]  Settings" : "  [3]  Settings"));
  Terminal::printAt(16, 10,
                    (selectedIndex == 4 ? "> [4]  Exit" : "  [4]  Exit"));

  Terminal::printAt(22, 5, "Guide: type number or arrow keys + Enter");
  Terminal::printAt(25, 5, "server: localHost:4444");
  Terminal::printAt(25, 70, "online");

  // Position the typing prompt and show the cursor
  Terminal::printAt(19, 10, "> ");
  Terminal::showCursor();
  Terminal::flush();
}

void MenuScreen::handleInput() {
  int key = Terminal::getch();

  // ASCII values for numbers 1-4
  if (key == '1') {
    selectedIndex = 1;
    nextScreen = "LoginScreen";
  } else if (key == '2') {
    selectedIndex = 2;
    nextScreen = "RegisterScreen";
  } else if (key == '3') {
    selectedIndex = 3;
    nextScreen = "SettingsScreen";
  } else if (key == '4') {
    selectedIndex = 4;
    Terminal::exitAlternateScreen();
    Terminal::showCursor();
    Terminal::clear();
    std::cout << "Exiting Flock...\n";
    exit(0);
  }
  // Handle arrow keys. Real terminals send a 3-character sequence for arrows:
  // ESC, '[', and 'A'/'B'/'C'/'D'
  else if (key == 27) { // ESC character
    int next1 = Terminal::getch();
    if (next1 == '[') {
      int next2 = Terminal::getch();
      if (next2 == 'A') { // Up arrow
        selectedIndex--;
        if (selectedIndex < 1)
          selectedIndex = 4;
      } else if (next2 == 'B') { // Down arrow
        selectedIndex++;
        if (selectedIndex > 4)
          selectedIndex = 1;
      }
    } else {
      // It was just the ESC key alone. Menu doesn't have a "back" target so we
      // ignore it.
    }
  }
  // Handle Enter key
  else if (key == 10 || key == 13) {
    if (selectedIndex == 1)
      nextScreen = "LoginScreen";
    else if (selectedIndex == 2)
      nextScreen = "RegisterScreen";
    else if (selectedIndex == 3)
      nextScreen = "SettingsScreen";
    else if (selectedIndex == 4) {
      Terminal::exitAlternateScreen();
      Terminal::showCursor();
      Terminal::clear();
      std::cout << "Exiting Flock...\n";
      exit(0);
    }
  }
}

std::string MenuScreen::getNextScreen() { return nextScreen; }
