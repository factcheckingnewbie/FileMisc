#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSplitter>
#include <QToolBar>
#include <QLabel>
#include <QDebug>
#include "cmdmaster_parser.h"

// NOTE: Added const CmdMasterConfig& config as parameter.
//       All debug output from your original has been preserved.
void addMenuItems(QMenu *menu, const QList<CmdMasterMenuItem> &items, QWidget *parent, const CmdMasterConfig &config) {
    for (const auto &item : items) {
        if (!item.label.isEmpty() && item.label != "---") {
            QAction *action = new QAction(item.label, parent);
            if (!item.shortcut.isEmpty())
                action->setShortcut(QKeySequence(item.shortcut));
            if (!item.icon.isEmpty())
                action->setIcon(QIcon::fromTheme(item.icon));
            action->setObjectName(item.action);

            // Command lookup and debug print for action
            QObject::connect(action, &QAction::triggered, [item, &config]() {
                qDebug() << "[Action Triggered]" << item.action << "Label:" << item.label;
                auto cmd = config.commands.value(item.action);
                if (!cmd.name.isEmpty()) {
                    qDebug() << "[Command Matched]" << cmd.name << "Alias:" << cmd.alias << "Desc:" << cmd.description;
                    printf("Command should be here instead of an stupd printf\n");
                }
            });

            menu->addAction(action);
        }
        if (item.label == "---") {
            menu->addSeparator();
        }
        if (!item.subItems.isEmpty()) {
            QMenu *submenu = new QMenu(item.label, parent);
            // RECURSIVE call, must pass config to all levels
            addMenuItems(submenu, item.subItems, parent, config);
            menu->addMenu(submenu);
        }
    }
}

int main(int argc, char *argv[]) {
    qputenv("QT_LOGGING_RULES", "*.debug=true");
    freopen("qtdebug.txt", "w", stderr);
    //    qCritical() << "THIS IS A qCritical OUTPUT TEST";
    QApplication app(argc, argv);
    QMainWindow mainWindow;

    // Parse config
    QString dslPath = "CommandMasterApp.dsl";
    CmdMasterConfig config;
    if (!CmdMasterParser::parseFile(dslPath, config)) {
        qWarning() << "[MAIN] Failed to parse config file!";
        return 1;
    }

    qDebug() << "[MAIN] Parsed windowTitle:" << config.windowTitle << "width:" << config.width << "height:" << config.height;
    mainWindow.setWindowTitle(config.windowTitle);
    mainWindow.resize(config.width, config.height);

    // Menubar
    QMenuBar *menuBar = mainWindow.menuBar();
    qDebug() << "[MAIN] Populating menubar, panels:" << config.menuBarPanels.size();
    for (const CmdMasterPanel &panel : config.menuBarPanels) {
        qDebug() << "[MAIN]  Panel type:" << panel.type << "menuItems:" << panel.menuItems.size();
        if (panel.type == "menubar") {
            for (const CmdMasterMenuItem &menuDef : panel.menuItems) {
                QMenu *menu = new QMenu(menuDef.label, &mainWindow);
                qDebug() << "[MAIN]   Creating QMenu:" << menuDef.label << "with subItems:" << menuDef.subItems.size();
                // Pass config to addMenuItems for command lookup in lambda
                addMenuItems(menu, menuDef.subItems, &mainWindow, config);
                menuBar->addMenu(menu);
                qDebug() << "[MAIN]   Added QMenu to menubar:" << menuDef.label;
            }
        }
    }

    // Buttonbar (QToolBar)
    if (!config.buttonBarPanels.isEmpty()) {
        for (const CmdMasterPanel &panel : config.buttonBarPanels) {
            if (panel.type == "buttonbar") {
                QToolBar *toolBar = new QToolBar("ButtonBar", &mainWindow);
                if (panel.attributes.contains("color")) {
                    toolBar->setStyleSheet(QString("QToolBar { background: %1; }").arg(panel.attributes["color"]));
                }
                for (const CmdMasterButton &btn : panel.buttons) {
                    QAction *action = new QAction(btn.label, &mainWindow);
                    if (!btn.icon.isEmpty())
                        action->setIcon(QIcon::fromTheme(btn.icon));
                    QObject::connect(action, &QAction::triggered, [btn, &config]() {
                        qDebug() << "[ButtonBar Triggered]" << btn.action << "Label:" << btn.label;
                        // Optional: lookup command for buttonbar too
                        auto cmd = config.commands.value(btn.action);
                        if (!cmd.name.isEmpty()) {
                            qDebug() << "[ButtonBar Command Matched]" << cmd.name << "Alias:" << cmd.alias << "Desc:" << cmd.description;
                        }
                    });
                    toolBar->addAction(action);
                }
                mainWindow.addToolBar(Qt::TopToolBarArea, toolBar);
            }
        }
    }

    // Splitter (QSplitter)
    if (!config.splitterPanels.isEmpty()) {
        // Only handling the first splitter for demo; extend as needed.
        const CmdMasterPanel &panel = config.splitterPanels[0];
        if (panel.type == "splitter") {
            QSplitter *splitter = new QSplitter(
                panel.splitter.orientation == "vertical" ? Qt::Vertical : Qt::Horizontal,
                &mainWindow
            );
            // For demo: add two placeholder widgets
            splitter->addWidget(new QLabel("Pane 1"));
            splitter->addWidget(new QLabel("Pane 2"));
            if (panel.splitter.sizes.size() >= 2)
                splitter->setSizes(panel.splitter.sizes);
            mainWindow.setCentralWidget(splitter);
        }
    } else {
        // fallback if no splitter defined
        mainWindow.setCentralWidget(new QLabel("No splitter defined in DSL"));
    }

    qDebug() << "[MAIN] Showing main window.";
    mainWindow.show();
    return app.exec();
}
