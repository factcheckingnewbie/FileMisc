#include <iostream>
#include <fstream>
#include <sstream>
#include "json_parser.h"
#include <nlohmann/json.hpp>
#include <filesystem>

// Helper: read indent size from dsl_config.json if present
int get_indent_size(int argc, char* argv[]) {
    // 1. Command line: --indent N
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "--indent") {
            return std::stoi(argv[i + 1]);
        }
    }
    // 2. dsl_config.json
    std::ifstream config_file("dsl_config.json");
    if (config_file) {
        nlohmann::json config;
        try {
            config_file >> config;
            if (config.contains("indent_size")) {
                return config["indent_size"].get<int>();
            }
        } catch (...) {}
    }
    // 3. Default
    return 2;
}

int main(int argc, char* argv[]) {
    std::istream* in = &std::cin;
    std::ifstream file;
    if (argc > 1 && std::string(argv[1]).rfind("--", 0) != 0) {
        file.open(argv[1]);
        if (!file) {
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
        in = &file;
    }

    nlohmann::ordered_json json;
    try {
        *in >> json;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        return 2;
    }

    int indent_size = get_indent_size(argc, argv);
    JSON2DSLWriter writer;
    writer.write_dsl(json, std::cout, indent_size);

    return 0;
}