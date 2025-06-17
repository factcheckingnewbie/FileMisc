cmake_minimum_required(VERSION 3.16)
project(FileMisc VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(KF5 REQUIRED COMPONENTS I18n KIO WidgetsAddons)

set(SOURCES
    main.cpp
    FilePanel.cpp
    TreePanel.cpp
    PanelWrapper.cpp
)

set(HEADERS
    FilePanel.h
    TreePanel.h
    PanelWrapper.h
)

add_executable(FileMisc ${SOURCES} ${HEADERS})

target_link_libraries(FileMisc
    Qt5::Core
    Qt5::Widgets
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::KIOFileWidgets
    KF5::WidgetsAddons
)

install(TARGETS FileMisc DESTINATION bin)
