cmake_minimum_required(VERSION 3.10)
project(Flock)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tell the compiler where to look for header files
include_directories(${CMAKE_SOURCE_DIR})

# Create the executable by combining main and your UI files
add_executable(flock 
    main.cpp 
    ui/LoadingScreen.cpp
    ui/MenuScreen.cpp
    ui/color.cpp
)

# Find and link the threading library safely
find_package(Threads REQUIRED)
target_link_libraries(flock Threads::Threads)