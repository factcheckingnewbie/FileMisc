#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>

// --- Data structures for config ---
struct CmdMasterCommand {
    QString name;
    QString alias;
    QString description;
    QStringList args;
    QString out;
    QString shell;
};

struct CmdMasterMenuItem {
    QString label;
    QString action;
    QString shortcut;
    QString icon;
    QList<CmdMasterMenuItem> subItems;
};

// New: Button for buttonbar
struct CmdMasterButton {
    QString label;
    QString action;
    QString icon;
};

// Support for splitter (minimal, just orientation and sizes)
struct CmdMasterSplitter {
    QString orientation; // "horizontal" or "vertical"
    QList<int> sizes;
};

// Expanded panel type, now supports menubar, buttonbar, splitter
struct CmdMasterPanel {
    QString type; // e.g., "menubar", "sidebar", "buttonbar", "splitter"
    QList<CmdMasterMenuItem> menuItems; // for menubar
    QList<CmdMasterButton> buttons;     // for buttonbar
    CmdMasterSplitter splitter;         // for splitter
    QMap<QString, QString> attributes;  // generic attributes (position, color, etc)
};

struct CmdMasterConfig {
    QString windowTitle;
    int width = 800;
    int height = 600;
    QList<CmdMasterPanel> menuBarPanels;
    QMap<QString, CmdMasterCommand> commands;
    QList<CmdMasterPanel> buttonBarPanels; // new for button bars
    QList<CmdMasterPanel> splitterPanels;  // new for splitters
};

// --- Parser API ---
class CmdMasterParser {
public:
    static bool parseFile(const QString &filePath, CmdMasterConfig &outConfig);
};
