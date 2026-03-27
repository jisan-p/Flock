#ifndef SCREEN_HPP
#define SCREEN_HPP

// Abstract Resource (an Interface)

#include <string>

class Screen {
public:
    virtual ~Screen() = default;

    virtual void render() = 0;  // renders the screen to the terminal

    virtual void handleInput() = 0;  // handles user input

    virtual std::string getNextScreen() = 0;  // returns the name of the next screen to transition to
};

#endif