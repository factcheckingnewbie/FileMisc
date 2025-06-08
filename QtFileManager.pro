QT += core widgets

CONFIG += c++17

TARGET = QtFileManager
TEMPLATE = app

SOURCES += \
    main.cpp \
    QtFileManager.cpp

HEADERS += \
    QtFileManager.h

# Additional configuration
CONFIG += debug_and_release
CONFIG += warn_on

# Remove duplicate filemanager.cpp and filemanager.h references
# Use only QtFileManager.cpp and QtFileManager.h for consistency
