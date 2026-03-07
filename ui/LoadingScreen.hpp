#ifndef LOADING_SCREEN_HPP
#define LOADING_SCREEN_HPP

#include "Screen.hpp"
#include <atomic>
#include <mutex>
#include <string>

class LoadingScreen : public Screen {
private:
  std::string nextScreen;

  // atomic so the dynamic thread and the network/sim thread can safely
  // read/write this flag without a data race.
  std::atomic<bool> isConnected;

  // All terminal writes (ANSI escape codes + cout) must be serialised through
  // this mutex so that the static thread and the dynamic thread never
  // interleave their output.
  std::mutex screenMutex;

public:
  LoadingScreen();
  void render() override;
  void handleInput() override;
  std::string getNextScreen() override;
};

#endif