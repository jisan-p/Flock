cmake_minimum_required(VERSION 3.10)

# Project name
project(FlockServer)

# Enforce C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the threading library (required for multi-threading)
find_package(Threads REQUIRED)

# Specify the executable and its source files
add_executable(FlockServer 
    main.cpp 
    Database/Database.cpp
    Network/TCPServer.cpp
)

# Tell the compiler where to find the header files (.hpp)
target_include_directories(FlockServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link the threading library to the executable
target_link_libraries(FlockServer PRIVATE Threads::Threads)