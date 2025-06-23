#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Read YAML file
    std::ifstream file("button.yaml");
    if (!file.is_open()) {
        std::cout << "Cannot open button.yaml" << std::endl;
        return 1;
    }
    
    YAML::Node root = YAML::Load(file);
    std::string label = root["label"].as<std::string>();
    
    std::cout << "Label from file: " << label << std::endl;
    
    QMainWindow window;
    QPushButton *button = new QPushButton(QString::fromStdString(label), &window);
    button->setGeometry(50, 50, 100, 30);
    
    window.show();
    return app.exec();
}
