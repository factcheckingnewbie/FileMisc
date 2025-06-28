#!/usr/bin/env bash

g++ -fPIC -std=c++17 test.cpp -o test $(pkg-config --cflags --libs Qt5Core Qt5Widgets yaml-cpp)

