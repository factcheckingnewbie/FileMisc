cmake_minimum_required(VERSION 3.16)
project(json2dsl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(json2dsl
    json_to_dsl.cpp
    json_to_dsl.h
    main.cpp
)

target_link_libraries(json2dsl Qt6::Core)
