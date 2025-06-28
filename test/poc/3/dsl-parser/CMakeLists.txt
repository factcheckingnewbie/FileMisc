# Line 1: Set the minimum CMake version required for the project.
cmake_minimum_required(VERSION 3.10)

# Line 2: Start a new project named CommandMasterLibDemo using C++.
project(CommandMasterLibDemo LANGUAGES CXX)

# Line 3-4: Set the C++ standard to 17 and require it.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Line 5: Find the required Qt5 modules (Core and Widgets).
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# Line 6-10: Define the parser library target with its implementation and header.
add_library(cmdmaster_parser
    cmdmaster_parser.cpp
    cmdmaster_parser.h
)

# Line 11-13: Link Qt5::Core to the parser library and specify include directories.
target_link_libraries(cmdmaster_parser PUBLIC Qt5::Core)
target_include_directories(cmdmaster_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Line 14-19: Define the main application executable target and include all relevant .cpp files.
add_executable(CommandMasterDemo
    main.cpp
    # Add other .cpp files here if you create them in the future.
)

# Line 20-22: Link the executable against Qt5::Core, Qt5::Widgets, and the parser library.
target_link_libraries(CommandMasterDemo PRIVATE Qt5::Core Qt5::Widgets cmdmaster_parser)
target_include_directories(CommandMasterDemo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Line 23-24: Install the DSL config file as a data file; this is optional and only needed if you use 'make install'.
install(FILES CommandMasterApp.dsl DESTINATION bin)

# Line 25-28: Enable Qt automoc/uic/rcc features for signal/slot and UI support.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
