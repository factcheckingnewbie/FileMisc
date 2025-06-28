// This file contains utility functions that assist the DSL parsing process, such as functions for handling whitespace and comments.

#include "utils.h"
#include <string>
#include <cctype>
#include <algorithm>

// Trims whitespace from both ends of a string
std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    auto end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Checks if a line is a comment
bool isComment(const std::string& line) {
    return line.find("//") == 0;
}

// Removes comments from a line
std::string removeComments(const std::string& line) {
    auto pos = line.find("//");
    return (pos == std::string::npos) ? line : line.substr(0, pos);
}

// Splits a string by a delimiter and trims each part
std::vector<std::string> splitAndTrim(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}