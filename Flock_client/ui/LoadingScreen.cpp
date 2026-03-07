#include "LoadingScreen.hpp"
#include "Terminal.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

LoadingScreen::LoadingScreen() {
    nextScreen = "";
    isConnected = false;
}

void LoadingScreen::render() {
    Terminal::clear();
    Terminal::hideCursor(); // Hide cursor so the animation doesn't flicker

    // 1. Draw the static skeleton (once on main thread)
    {
        std::lock_guard<std::mutex> lock(Terminal::screenMutex);
        Terminal::printAt(5, 38, "Flock");
        Terminal::printAt(8, 30, "connecting to server...");
        Terminal::printAt(12, 35, "(Please wait)");
        Terminal::printAt(20, 5, "version 0.1");
        Terminal::printAt(20, 55, "made with C++ with sockets");
        Terminal::flush();
    } // lock released so dynamic thread can use it

    // 2. The dynamic pulsing dots animation in a separate thread
    std::thread dynamicThread([this]() {
        int dotCount = 0;
        while (!isConnected) {
            {
                std::lock_guard<std::mutex> lock(Terminal::screenMutex);
                Terminal::moveCursor(10, 35); // Teleport to the animation row
                
                std::string dots = "";
                for(int i = 0; i < dotCount; i++) dots += ". ";
                // Pad with spaces to visually erase the dots when it resets
                for(int i = dotCount; i < 5; i++) dots += "  "; 
                
                std::cout << dots;
                Terminal::flush(); // Force the terminal to draw immediately
            }

            // Wait 400ms before drawing the next frame
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            
            dotCount++;
            if (dotCount > 5) dotCount = 0;
        }

        // Clean up the animation dots once connected
        {
            std::lock_guard<std::mutex> lock(Terminal::screenMutex);
            Terminal::printAt(10, 35, "          "); 
            Terminal::flush();
        }
    });

    // Note: In your final app, this flag is set by your Network Thread.
    // For testing the UI, we simulate a connection after ~3 seconds.
    int simulatedTime = 0;
    while (simulatedTime <= 7) {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        simulatedTime++;
    }
    isConnected = true; 
    
    // Wait for the animation thread to finish cleanly
    dynamicThread.join();

    // 3. Connection successful, prepare to transition
    nextScreen = "MenuScreen"; 
    Terminal::showCursor(); // Always clean up and show the cursor before leaving
}

void LoadingScreen::handleInput() {
    // We intentionally leave this empty. 
    // We do not want to process keys while connecting.
}

std::string LoadingScreen::getNextScreen() {
    return nextScreen;
}