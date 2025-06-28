#include <iostream>
#include <fstream>
#include <sstream>
#include "dsl_parser.h"

int main(int argc, char* argv[]) {
    std::istream* in = &std::cin;
    std::ifstream file;
    if (argc > 1) {
        file.open(argv[1]);
        if (!file) {
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
        in = &file;
    }

    DSLParser parser;
    auto root = parser.parse(*in);
    auto json = parser.to_json(root);

    std::cout << json.dump(2) << std::endl;
    return 0;
}