#ifndef MENU_SCREEN_HPP
#define MENU_SCREEN_HPP

#include "Screen.hpp"
#include <string>

class MenuScreen : public Screen {
private:
  std::string nextScreen;
  bool firstRender;   // true on first call to render(); draws static skeleton
  int selectedIndex;

public:
  MenuScreen();
  void render() override;
  void handleInput() override;
  std::string getNextScreen() override;
};

#endif