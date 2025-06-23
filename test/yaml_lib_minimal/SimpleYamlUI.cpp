#include "SimpleYamlUI.h"
#include <yaml-cpp/yaml.h>
#include <QLayout>
#include <QMenuBar>
#include <QDebug>
#include <QFile>
#include <QAction>

SimpleYamlUI::SimpleYamlUI(QObject *parent) : QObject(parent) {
}

SimpleYamlUI::~SimpleYamlUI() {
    clearAll();
}

void SimpleYamlUI::clearAll() {
    qDeleteAll(m_buttons);
    m_buttons.clear();
    qDeleteAll(m_menus);
    m_menus.clear();
}

bool SimpleYamlUI::loadFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Cannot open file: %1").arg(filename));
        return false;
    }
    
    QString content = file.readAll();
    file.close();
    return loadString(content);
}

bool SimpleYamlUI::loadString(const QString &yamlContent) {
    clearAll();
    
    try {
        YAML::Node root = YAML::Load(yamlContent.toStdString());
        
        // Parse buttons
        if (root["buttons"]) {
            for (const auto& node : root["buttons"]) {
                QString id = QString::fromStdString(node["id"].as<std::string>(""));
                QString label = QString::fromStdString(node["label"].as<std::string>(""));
                
                if (!id.isEmpty() && !label.isEmpty()) {
                    QPushButton *btn = new QPushButton(label);
                    btn->setObjectName(id);
                    
                    // Connect signal
                    connect(btn, &QPushButton::clicked, [this, id]() {
                        emit buttonClicked(id);
                    });
                    
                    m_buttons.append(btn);
                }
            }
        }
        
        // Parse menus
        if (root["menus"]) {
            for (const auto& node : root["menus"]) {
                QString id = QString::fromStdString(node["id"].as<std::string>(""));
                QString label = QString::fromStdString(node["label"].as<std::string>(""));
                
                if (!id.isEmpty() && !label.isEmpty()) {
                    QMenu *menu = new QMenu(label);
                    menu->setObjectName(id);
                    
                    // Parse menu items
                    if (node["items"]) {
                        for (const auto& item : node["items"]) {
                            QString itemId = QString::fromStdString(item["id"].as<std::string>(""));
                            QString itemLabel = QString::fromStdString(item["label"].as<std::string>(""));
                            
                            if (!itemId.isEmpty() && !itemLabel.isEmpty()) {
                                QAction *action = menu->addAction(itemLabel);
                                action->setObjectName(itemId);
                                
                                connect(action, &QAction::triggered, [this, itemId]() {
                                    emit menuTriggered(itemId);
                                });
                            }
                        }
                    }
                    
                    m_menus.append(menu);
                }
            }
        }
        
        return true;
        
    } catch (const YAML::Exception& e) {
        emit error(QString("YAML parse error: %1").arg(e.what()));
        return false;
    }
}

void SimpleYamlUI::addButtonsTo(QLayout *layout) {
    for (QPushButton *btn : m_buttons) {
        layout->addWidget(btn);
    }
}

void SimpleYamlUI::addMenusTo(QMenuBar *menuBar) {
    for (QMenu *menu : m_menus) {
        menuBar->addMenu(menu);
    }
}

// THIS LINE MUST BE AT THE VERY END OF THE FILE
