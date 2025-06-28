// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// Function to trim whitespace from both ends of a string
std::string trim(const std::string& str);

// Function to split a string by a delimiter
std::vector<std::string> split(const std::string& str, char delimiter);

// Function to check if a line is a comment
bool isComment(const std::string& line);

// Function to preserve indentation for a given line
std::string preserveIndentation(const std::string& line);

#endif // UTILS_H