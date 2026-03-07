#include "ui/LoadingScreen.hpp"
#include "ui/Terminal.hpp"
#include <iostream>

int main() {
    // 1. Create the screen object
    LoadingScreen splash;
    
    // 2. Run the blocking render loop (the animation)
    splash.render();
    
    // 3. Clean up after the animation finishes
    Terminal::clear();
    Terminal::moveCursor(1, 1);
    
    // Prove that the state machine logic works
    std::cout << "Loading complete!\n";
    std::cout << "The ScreenManager should now load: " << splash.getNextScreen() << "\n";
    
    return 0;
}