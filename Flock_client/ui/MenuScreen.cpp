#include "MenuScreen.hpp"
#include "Terminal.hpp"
#include "color.hpp"
#include <iostream>
#include <mutex>

MenuScreen::MenuScreen() {
  nextScreen    = "";
  selectedIndex = 1;
  firstRender   = true;
}


void MenuScreen::render() {
  std::lock_guard<std::mutex> lock(Terminal::screenMutex);

  Terminal::hideCursor();

  if (firstRender) {
    // Draw static skeleton exactly once
    Terminal::clear();
    Terminal::printAt(5,  38, "Flock");
    Terminal::printAt(22,  5, "Guide: type number or arrow keys + Enter");
    Terminal::printAt(25,  5, "server: localHost:4444");
    Terminal::printAt(25, 70, "online");
    Terminal::printAt(19, 10, "> ");
    firstRender = false;
  }

  // Redraw only the option lines in-place (mutex already held)
  Terminal::printAt(10, 10,
    (selectedIndex == 1 ? Color::cyan("> [1]  Login")          : "  [1]  Login"));
  Terminal::printAt(12, 10,
    (selectedIndex == 2 ? Color::cyan("> [2]  Create Account") : "  [2]  Create Account"));
  Terminal::printAt(14, 10,
    (selectedIndex == 3 ? Color::cyan("> [3]  Settings")       : "  [3]  Settings"));
  Terminal::printAt(16, 10,
    (selectedIndex == 4 ? Color::cyan("> [4]  Exit")           : "  [4]  Exit"));

  Terminal::showCursor();
  Terminal::flush();
}



void MenuScreen::handleInput() {
  int key = Terminal::getch();

  if (key == '1') {
    selectedIndex = 1;
    nextScreen    = "LoginScreen";

  } else if (key == '2') {
    selectedIndex = 2;
    nextScreen    = "RegisterScreen";

  } else if (key == '3') {
    selectedIndex = 3;
    nextScreen    = "SettingsScreen";

  } else if (key == '4') {
    Terminal::exitAlternateScreen();
    Terminal::showCursor();
    Terminal::clear();
    std::cout << "Exiting Flock...\n";
    exit(0);

  } else if (key == 27) {
    // Arrow keys send ESC + '[' + letter
    int next1 = Terminal::getch();
    if (next1 == '[') {
      int next2 = Terminal::getch();
      if (next2 == 'A') {          // Up
        selectedIndex--;
        if (selectedIndex < 1) selectedIndex = 4;
      } else if (next2 == 'B') {   // Down
        selectedIndex++;
        if (selectedIndex > 4) selectedIndex = 1;
      }
    }

  } else if (key == 10 || key == 13) {
    // Enter — confirm selection
    if      (selectedIndex == 1) nextScreen = "LoginScreen";
    else if (selectedIndex == 2) nextScreen = "RegisterScreen";
    else if (selectedIndex == 3) nextScreen = "SettingsScreen";
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
