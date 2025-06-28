cmake_minimum_required(VERSION 3.10)
project(CommandMasterDSL2YAML LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    parseDSLCmd2yaml.cpp
)

    set(HEADERS
    parseDSL2yaml.h
)

add_executable(parseDSL2yaml ${SOURCES} ${HEADERS})

target_link_libraries(parseDSL2yaml Qt5::Core)
