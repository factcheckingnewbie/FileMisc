#include "dsl_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not write to file: " + filename);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    try {
        std::string inputFile = "input.dsl";
        std::string outputFile = "output.json";
        
        if (argc > 1) {
            inputFile = argv[1];
        }
        if (argc > 2) {
            outputFile = argv[2];
        }
        
        std::cout << "Parsing DSL file: " << inputFile << std::endl;
        
        // Read DSL content
        std::string dslContent = readFile(inputFile);
        
        // Parse to JSON
        DSLParser::DSLToJsonParser parser;
        std::string jsonOutput = parser.parseToString(dslContent);
        
        // Write JSON output
        writeFile(outputFile, jsonOutput);
        
        std::cout << "Successfully parsed to: " << outputFile << std::endl;
        std::cout << "\nJSON Output:\n" << jsonOutput << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
