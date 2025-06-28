cmake_minimum_required(VERSION 3.16)
project(DSLParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_executable(${PROJECT_NAME}
    dsl_parser.cpp
    dsl_parser.h
    main.cpp
)

target_link_libraries(${PROJECT_NAME} Qt6::Core)

# For Qt5 compatibility:
# find_package(Qt5 REQUIRED COMPONENTS Core)
# target_link_libraries(${PROJECT_NAME} Qt5::Core)
