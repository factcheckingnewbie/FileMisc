cmake_minimum_required(VERSION 3.10)
project(DSLParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JsonCpp package
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

# If not found with pkg-config, try to find it manually
if(NOT JSONCPP_FOUND)
    find_path(JSONCPP_INCLUDE_DIR json/json.h
        PATHS /usr/include/jsoncpp /usr/local/include/jsoncpp)
    find_library(JSONCPP_LIBRARY jsoncpp
        PATHS /usr/lib /usr/local/lib)
    
    if(JSONCPP_INCLUDE_DIR AND JSONCPP_LIBRARY)
        set(JSONCPP_FOUND TRUE)
        set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR})
        set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
    endif()
endif()

if(NOT JSONCPP_FOUND)
    message(FATAL_ERROR "JsonCpp not found. Please install libjsoncpp-dev")
endif()

# Add executable
add_executable(dsl_parser 
    main.cpp
    dsl_parser.cpp
)

# Include directories
target_include_directories(dsl_parser PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${JSONCPP_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(dsl_parser 
    ${JSONCPP_LIBRARIES}
)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(dsl_parser PRIVATE -Wall -Wextra -Wpedantic)
endif()
