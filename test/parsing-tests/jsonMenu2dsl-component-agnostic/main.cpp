#include <QCoreApplication>
#include <QTextStream>
#include <iostream>
#include "json_to_dsl.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <json-file>" << std::endl;
        return 1;
    }
    
    JsonToDSL converter;
    QString dsl = converter.convertFile(argv[1]);
    
    if (dsl.isEmpty()) {
        std::cerr << "Error: Failed to convert JSON file" << std::endl;
        return 1;
    }
    
    QTextStream out(stdout);
    out << dsl;
    
    return 0;
}
