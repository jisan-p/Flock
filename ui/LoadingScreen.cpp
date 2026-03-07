#include "LoadingScreen.hpp"
#include "Terminal.hpp"
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

LoadingScreen::LoadingScreen() {
    nextScreen = "";
    isConnected = false;
}

// ---------------------------------------------------------------------------
// render()
//
//  Two parts of the screen are drawn concurrently:
//
//  [STATIC]  – drawn ONCE on the calling thread, then never touched again.
//              These are labels / decorations that never change.
//
//  [DYNAMIC] – runs in a background thread in a tight loop.
//              Only this thread updates the animation row.
//
//  Both threads must take `screenMutex` before touching std::cout /
//  any ANSI escape codes, so their output never interleaves.
// ---------------------------------------------------------------------------

void LoadingScreen::render() {
  Terminal::clear();
  Terminal::hideCursor();

  // ── STATIC PART (drawn once, right here on the main thread) ──────────────
  {
    std::lock_guard<std::mutex> lock(screenMutex);
    Terminal::printAt(5, 38, "Flock");
    Terminal::printAt(8, 30, "connecting to server...");
    Terminal::printAt(12, 35, "(Please wait)");
    Terminal::printAt(20, 5, "version 0.1");
    Terminal::printAt(20, 55, "made with C++ with sockets");
    Terminal::flush();
  } // lock released here – the dynamic thread may now proceed

  // ── DYNAMIC PART (runs in its own thread) ─────────────────────────────────
  // We capture `this` so the lambda can access isConnected and screenMutex.
  std::thread dynamicThread([this]() {
    int dotCount = 0;

    while (!isConnected) { // atomic read – no race
      {
        std::lock_guard<std::mutex> lock(screenMutex);
        Terminal::moveCursor(10, 35); // go to the animation row

        // Build a string like ". . . " that resets after 5 dots
        std::string dots;
        for (int i = 0; i < dotCount; i++)
          dots += ". ";
        for (int i = dotCount; i < 5; i++)
          dots += "  "; // erase old dots

        std::cout << dots;
        Terminal::flush();
      } // release lock while sleeping

      std::this_thread::sleep_for(std::chrono::milliseconds(400));

      dotCount++;
      if (dotCount > 5)
        dotCount = 0;
    }

    // Final frame: clear the animation row so it looks clean on exit
    {
      std::lock_guard<std::mutex> lock(screenMutex);
      Terminal::printAt(10, 35, "          "); // erase dots
      Terminal::flush();
    }
  });

  // ── SIMULATE connection (replace this block with your real network call) ──
  // In production: your Network Thread sets isConnected = true.
  // Here we just wait ~3 s so you can see both threads working together.
  {
    int ticks = 0;
    while (ticks < 8) {
      std::this_thread::sleep_for(std::chrono::milliseconds(400));
      ticks++;
    }
    isConnected = true; // atomic write – wakes up the dynamic thread's loop
  }

  // Wait for the dynamic thread to finish its final frame and exit cleanly
  dynamicThread.join();

  // Transition to the next screen
  nextScreen = "MenuScreen";
  Terminal::showCursor();
}

void LoadingScreen::handleInput() {
  // Intentionally empty – no input while connecting.
}

std::string LoadingScreen::getNextScreen() { return nextScreen; }