cmake_minimum_required(VERSION 3.5)
project(KIOCommander)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(KF5KIO REQUIRED)
find_package(KF5CoreAddons REQUIRED)
find_package(KF5I18n REQUIRED)
find_package(KF5KIO REQUIRED)




add_executable(FileMisc main.cpp FilePanel.cpp DirOnlyProxyModel.cpp)

include_directories(/usr/include/KF5/KIOFileWidgets)
target_link_libraries(FileMisc Qt5::Widgets KF5::KIOCore KF5::KIOWidgets KF5::KIOFileWidgets KF5::CoreAddons KF5::I18n)
