QT += core widgets

CONFIG += c++17

TARGET = QtFileManagerAlt
TEMPLATE = app

SOURCES += \
    main.cpp \
    QtFileManager.cpp

HEADERS += \
    QtFileManager.h

# Additional configuration
CONFIG += debug_and_release
CONFIG += warn_on