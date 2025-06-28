cmake_minimum_required(VERSION 3.16)
project(SimpleYamlUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Find dependencies
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(yaml-cpp REQUIRED)

# Library source files
set(SOURCES
    SimpleYamlUI.cpp
)

set(HEADERS
    SimpleYamlUI.h
)

# Create shared library
add_library(SimpleYamlUI SHARED ${SOURCES} ${HEADERS})

# Create static library
add_library(SimpleYamlUI_static STATIC ${SOURCES} ${HEADERS})
set_target_properties(SimpleYamlUI_static PROPERTIES OUTPUT_NAME SimpleYamlUI)

# Link dependencies
target_link_libraries(SimpleYamlUI
    PUBLIC
        Qt5::Core
        Qt5::Widgets
        yaml-cpp
)

target_link_libraries(SimpleYamlUI_static
    PUBLIC
        Qt5::Core
        Qt5::Widgets
        yaml-cpp
)

# Install rules
install(TARGETS SimpleYamlUI SimpleYamlUI_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES SimpleYamlUI.h
    DESTINATION include
)

# Create pkg-config file
configure_file(SimpleYamlUI.pc.in SimpleYamlUI.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/SimpleYamlUI.pc
    DESTINATION lib/pkgconfig
)
