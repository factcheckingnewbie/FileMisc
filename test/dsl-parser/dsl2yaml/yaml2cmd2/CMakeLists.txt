cmake_minimum_required(VERSION 3.10)
project(CommandMasterDSL2YAML LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(yaml-cpp REQUIRED)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    parseYAMLCmd2DSL.cpp
)

    set(HEADERS
    parseYAMLCmd2DSL.h
)

add_executable(parseYAMLCmd2DSL ${SOURCES} ${HEADERS})

target_link_libraries(parseYAMLCmd2DSL 
    Qt5::Core
    yaml-cpp
)
