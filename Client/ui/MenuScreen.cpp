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
    Terminal::clear();

    // ── ASCII art logo (green) ──
    Terminal::printAt(2, 5, Color::yellow(" _____ _            _    "));
    Terminal::printAt(3, 5, Color::yellow("|  ___| | ___   ___| | __"));
    Terminal::printAt(4, 5, Color::yellow("| |_  | |/ _ \\ / __| |/ /"));
    Terminal::printAt(5, 5, Color::yellow("|  _| | | (_) | (__|   < "));
    Terminal::printAt(6, 5, Color::yellow("|_|   |_|\\___/ \\___|_|\\_\\"));

    // ── Tagline next to logo ──
    Terminal::printAt(5, 33, Color::green("github.com/jisan-p/Flock"));
    Terminal::printAt(6, 33, Color::cyan("Terminal chat application."));

    // ── Input prompt ──
    Terminal::printAt(15, 3, "> ");

    // ── Bottom shortcut bar ──
    Terminal::printAt(20, 3,
      Color::bold("\xe2\x86\x91\xe2\x86\x93") + "  |  " +
      Color::bold("Enter") + " Select  |  " +
      Color::bold("Q") + " Quit");

    firstRender = false;
  }

  // ── Menu items ──
  struct MenuItem { const char* label; const char* desc; };
  MenuItem items[] = {
    { "Login",            "Sign in to your account"    },
    { "Create Account",   "Register a new account"     },
    { "Settings",         "Configure preferences"      },
    { "Exit",             "Quit the application"       },
  };

  for (int i = 0; i < 4; i++) {
    int row = 9 + i;
    int idx = i + 1;

    char numBuf[8];
    snprintf(numBuf, sizeof(numBuf), "%d.", idx);

    // Pad label to 18 chars for alignment
    std::string label(items[i].label);
    while (label.size() < 18) label += ' ';

    std::string line;
    if (idx == selectedIndex) {
      line = Color::bold(Color::yellow(
        std::string("\xe2\x96\xb8 ") + numBuf + " " + label)) +
        Color::bold(Color::yellow(std::string(items[i].desc)));
    } else {
      line = std::string("  ") + numBuf + " " + label + items[i].desc;
    }

    // Clear the row first, then print
    Terminal::printAt(row, 3, "                                                            ");
    Terminal::printAt(row, 3, line);
  }

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

  } else if (key == '4' || key == 'q' || key == 'Q') {
    if (key == '4' || key == 'q' || key == 'Q') {
      Terminal::exitAlternateScreen();
      Terminal::showCursor();
      Terminal::clear();
      std::cout << "Exiting Flock...\n";
      exit(0);
    }

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
