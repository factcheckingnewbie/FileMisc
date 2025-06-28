cmake_minimum_required(VERSION 3.10)
project(dsl2json VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(json2dsl
    src/main.cpp
    src/json_parser.cpp
)

# Optionally, link pthread if your code or dependencies require threading
# target_link_libraries(dsl2json
