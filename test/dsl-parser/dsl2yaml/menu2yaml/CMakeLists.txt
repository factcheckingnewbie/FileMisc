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
    parseDSLMenu2yaml.cpp        # <--- ADD THIS LINE
)

set(HEADERS
    parseDSLMenu2yaml.h          # <--- ADD THIS LINE
)

add_executable(parseDSLMenu2yaml ${SOURCES} ${HEADERS})

target_link_libraries(parseDSLMenu2yaml Qt5::Core)
