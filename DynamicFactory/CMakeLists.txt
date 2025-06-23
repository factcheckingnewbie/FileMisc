cmake_minimum_required(VERSION 3.10)
project(DynamicYamlPanelDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt (change version as needed, e.g., 5.15 or 6.2)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Find yaml-cpp
find_package(yaml-cpp REQUIRED)

add_executable(DynamicYamlPanelDemo
    main.cpp
    DynamicUiFactory.cpp
    DynamicUiFactory.h
)

target_link_libraries(DynamicYamlPanelDemo
    Qt5::Widgets
    yaml-cpp
)

# Install YAML example file
install(FILES buttons.yaml DESTINATION ${CMAKE_BINARY_DIR})
