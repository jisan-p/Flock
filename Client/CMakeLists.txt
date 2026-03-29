cmake_minimum_required(VERSION 3.10)
project(Flock)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tell the compiler where to look for header files
include_directories(${CMAKE_SOURCE_DIR})

# Create the executable by combining main and all source files
add_executable(flock 
    main.cpp 
    ui/LoadingScreen.cpp
    ui/MenuScreen.cpp
    ui/LoginScreen.cpp
    ui/RegisterScreen.cpp
    ui/HomeScreen.cpp
    ui/ChatRoomScreen.cpp
    ui/color.cpp
    ui/Terminal.cpp
    network/TCPClient.cpp
)

# Find and link the threading library safely
find_package(Threads REQUIRED)
target_link_libraries(flock Threads::Threads)