cmake_minimum_required(VERSION 3.5)
project(KIOCommander)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6Widgets REQUIRED)
find_package(KF6KIO REQUIRED)
find_package(KF6CoreAddons REQUIRED)
find_package(KF6I18n REQUIRED)

add_executable(FileMisc main.cpp FilePanel.cpp)
target_link_libraries(FileMisc  Qt6::Widgets KF6::KIOCore KF6::KIOWidgets KF6::CoreAddons KF6::I18n)
