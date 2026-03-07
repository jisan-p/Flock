#include "ui/LoadingScreen.hpp"
#include "ui/MenuScreen.hpp"
#include "ui/Terminal.hpp"
#include <iostream>

int main() {
  // Enter the alternate screen buffer: our drawing surface is completely
  // isolated from the user's scrollback history - no scrolling ever occurs.
  Terminal::enterAlternateScreen();
  Terminal::hideCursor();

  Screen *currentScreen = new LoadingScreen();

  while (currentScreen != nullptr) {
    currentScreen->render();
    currentScreen->handleInput();

    std::string next = currentScreen->getNextScreen();

    if (next != "") { // A transition was requested
      delete currentScreen;
      if (next == "MenuScreen") {
        currentScreen = new MenuScreen();
      } else {
        Terminal::exitAlternateScreen(); // restore real terminal before
                                         // printing
        Terminal::showCursor();
        std::cout << "Attempted to transition to unimplemented screen: " << next
                  << "\n";
        return 1;
      }
    }
  }

  // Restore the user's normal terminal before exit
  Terminal::exitAlternateScreen();
  Terminal::showCursor();
  return 0;
}