QT += core widgets

CONFIG += c++17

TARGET = QtFileManager
TEMPLATE = app

SOURCES += \
    main.cpp \
    filemanager.cpp

HEADERS += \
    filemanager.h

# Additional configuration
CONFIG += debug_and_release
CONFIG += warn_on
