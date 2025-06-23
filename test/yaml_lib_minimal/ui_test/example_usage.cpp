#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include "SimpleYamlUI.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    QWidget *central = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(central);
    window.setCentralWidget(central);
    
    // Create YAML UI loader
    SimpleYamlUI yamlUI(&window);
    
    // Connect signals
    QObject::connect(&yamlUI, &SimpleYamlUI::buttonClicked, [](const QString &id) {
        QMessageBox::information(nullptr, "Button Clicked", QString("Button ID: %1").arg(id));
    });
    
    QObject::connect(&yamlUI, &SimpleYamlUI::menuTriggered, [](const QString &id) {
        QMessageBox::information(nullptr, "Menu Triggered", QString("Menu ID: %1").arg(id));
    });
    
    QObject::connect(&yamlUI, &SimpleYamlUI::error, [](const QString &msg) {
        QMessageBox::critical(nullptr, "Error", msg);
    });
    
    // Load UI from file
    if (yamlUI.loadFile("ui.yaml")) {
        yamlUI.addButtonsTo(layout);
        yamlUI.addMenusTo(window.menuBar());
    }
    
    window.setWindowTitle("Simple YAML UI Example");
    window.resize(400, 300);
    window.show();
    
    return app.exec();
}
