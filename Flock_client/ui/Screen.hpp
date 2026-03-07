#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <string>

class Screen {
public:
  virtual ~Screen() = default;

  // Renders the screen to the terminal
  virtual void render() = 0;

  // Handles user input
  virtual void handleInput() = 0;

  // Returns the name of the next screen to transition to.
  // Return an empty string or the current screen's name if no transition is
  // needed.
  virtual std::string getNextScreen() = 0;
};

#endif