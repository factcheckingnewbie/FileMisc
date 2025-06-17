cmake_minimum_required(VERSION 3.16)
project(kio-commander)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Enable all warnings and treat warnings as errors in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
        # Enable sanitizers for better error detection
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/W4 /WX)
    endif()
endif()

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(KF5 REQUIRED COMPONENTS KIO WidgetsAddons)

add_executable(kio-commander
    main.cpp
    TreePanel.cpp
    TreePanel.h
    FilePanel.cpp
    FilePanel.h
)

target_link_libraries(kio-commander
    Qt5::Core
    Qt5::Widgets
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::KIOFileWidgets
    KF5::WidgetsAddons
)

# Add compile definitions for better debugging
target_compile_definitions(kio-commander PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Debug>:QT_MESSAGELOGCONTEXT>
)
