#include "cmdmaster_parser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

// --- Helpers ---
static int getIndent(const QString &line) {
    int i = 0;
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
    return i;
}

static QList<int> parseSizes(const QString &sizesStr) {
    QList<int> sizes;
    for (const QString &s : sizesStr.split(',', Qt::SkipEmptyParts)) {
        bool ok = false;
        int v = s.trimmed().toInt(&ok);
        if (ok) sizes.append(v);
    }
    return sizes;
}

// Debug utility to print a menu structure recursively
static void printMenuItems(const QList<CmdMasterMenuItem> &items, int level = 0) {
    for (const auto &item : items) {
        QString indent(level * 2, ' ');
        qDebug() << indent << "Item label:" << item.label << "action:" << item.action << "shortcut:" << item.shortcut << "icon:" << item.icon << "subItems:" << item.subItems.size();
        if (!item.subItems.isEmpty()) {
            printMenuItems(item.subItems, level + 1);
        }
    }
}
static void printButtons(const QList<CmdMasterButton> &buttons, int level = 0) {
    QString indent(level * 2, ' ');
    for (const auto &btn : buttons) {
        qDebug() << indent << "Button label:" << btn.label << "action:" << btn.action << "icon:" << btn.icon;
    }
}
static void printSplitter(const CmdMasterSplitter &splitter, int level = 0) {
    QString indent(level * 2, ' ');
    qDebug() << indent << "Splitter orientation:" << splitter.orientation << " sizes:" << splitter.sizes;
}

// --- Parser ---
bool CmdMasterParser::parseFile(const QString &filePath, CmdMasterConfig &outConfig) {
    qDebug() << "[Parser] Opening file:" << filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Parser] Failed to open file:" << filePath;
        return false;
    }
    QTextStream in(&file);

    QList<CmdMasterPanel> menuBarPanelsBuffer;
    QList<CmdMasterPanel> buttonBarPanelsBuffer;
    QList<CmdMasterPanel> splitterPanelsBuffer;
    QMap<QString, CmdMasterCommand> commandsBuffer;

    QString currentSection;
    CmdMasterPanel currentPanel;
    QList<CmdMasterMenuItem> *currentMenuList = nullptr;
    QList<CmdMasterButton> *currentButtonList = nullptr;
    int lastMenuIdx = -1;
    int lineNumber = 0;
    bool menubarActive = false;
    bool buttonbarActive = false;

    while (!in.atEnd()) {
        QString rawLine = in.readLine();
        ++lineNumber;
        QString line = rawLine.trimmed();
        int indent = getIndent(rawLine);

        qDebug() << "[Parser] Line" << lineNumber << ":" << rawLine;

        if (line.isEmpty() || line.startsWith('#')) {
            qDebug() << "[Parser] Skipping empty or comment line.";
            continue;
        }

        // --- Window ---
        if (line.startsWith("Window")) {
            QRegularExpression re(R"(Window\s*\"([^\"]+)\")");
            auto match = re.match(line);
            if (match.hasMatch()) {
                outConfig.windowTitle = match.captured(1);
                qDebug() << "[Parser] Parsed window title:" << outConfig.windowTitle;
            }
            QRegularExpression reDim(R"(width:(\d+)\s*height:(\d+))");
            auto m2 = reDim.match(line);
            if (m2.hasMatch()) {
                outConfig.width = m2.captured(1).toInt();
                outConfig.height = m2.captured(2).toInt();
                qDebug() << "[Parser] Parsed window dimensions:" << outConfig.width << "x" << outConfig.height;
            }
        }
        // --- Panel menubar ---
        else if (line.startsWith("Panel menubar")) {
            // Flush any existing menubar
            if (menubarActive && !currentPanel.menuItems.isEmpty()) {
                qDebug() << "[Parser] End of previous menubar panel, buffering panel with" << currentPanel.menuItems.size() << "menus.";
                printMenuItems(currentPanel.menuItems, 1);
                menuBarPanelsBuffer.append(currentPanel);
                currentPanel = CmdMasterPanel();
                currentMenuList = nullptr;
                lastMenuIdx = -1;
            }
            // Flush menubar if active before switching to any other panel type
            if (!menubarActive && !currentPanel.menuItems.isEmpty()) {
                qDebug() << "[Parser] End of non-active menubar panel, buffering panel with" << currentPanel.menuItems.size() << "menus.";
                printMenuItems(currentPanel.menuItems, 1);
                menuBarPanelsBuffer.append(currentPanel);
                currentPanel = CmdMasterPanel();
                currentMenuList = nullptr;
                lastMenuIdx = -1;
            }
            menubarActive = true;
            buttonbarActive = false;
            currentSection = "menubar";
            currentPanel = CmdMasterPanel();
            currentPanel.type = "menubar";
            currentPanel.attributes.clear();
            // Parse attributes (e.g. position, color)
            QRegularExpression attrRe(R"((\w+):([^\s]+))");
            auto it = attrRe.globalMatch(line);
            while (it.hasNext()) {
                auto m = it.next();
                currentPanel.attributes[m.captured(1)] = m.captured(2);
            }
            currentMenuList = &currentPanel.menuItems;
            lastMenuIdx = -1;
            qDebug() << "[Parser] New Panel menubar started";
        }
        // --- Panel buttonbar ---
        else if (line.startsWith("Panel buttonbar")) {
            // FLUSH any menubar panel if active before switching to buttonbar
            if (menubarActive && !currentPanel.menuItems.isEmpty()) {
                qDebug() << "[Parser] End of menubar panel, buffering panel with" << currentPanel.menuItems.size() << "menus.";
                printMenuItems(currentPanel.menuItems, 1);
                menuBarPanelsBuffer.append(currentPanel);
                menubarActive = false;
                currentPanel = CmdMasterPanel();
                currentMenuList = nullptr;
                lastMenuIdx = -1;
            }
            if (buttonbarActive && !currentPanel.buttons.isEmpty()) {
                qDebug() << "[Parser] End of previous buttonbar panel, buffering panel with" << currentPanel.buttons.size() << "buttons.";
                printButtons(currentPanel.buttons, 1);
                buttonBarPanelsBuffer.append(currentPanel);
            }
            buttonbarActive = true;
            menubarActive = false;
            currentSection = "buttonbar";
            currentPanel = CmdMasterPanel();
            currentPanel.type = "buttonbar";
            currentPanel.attributes.clear();
            QRegularExpression attrRe(R"((\w+):([^\s]+))");
            auto it = attrRe.globalMatch(line);
            while (it.hasNext()) {
                auto m = it.next();
                currentPanel.attributes[m.captured(1)] = m.captured(2);
            }
            currentButtonList = &currentPanel.buttons;
            qDebug() << "[Parser] New Panel buttonbar started";
        }
        // --- Panel splitter ---
        else if (line.startsWith("Panel splitter")) {
            // FLUSH any menubar panel if active before switching to splitter
            if (menubarActive && !currentPanel.menuItems.isEmpty()) {
                qDebug() << "[Parser] End of menubar panel, buffering panel with" << currentPanel.menuItems.size() << "menus.";
                printMenuItems(currentPanel.menuItems, 1);
                menuBarPanelsBuffer.append(currentPanel);
                menubarActive = false;
                currentPanel = CmdMasterPanel();
                currentMenuList = nullptr;
                lastMenuIdx = -1;
            }
            // FLUSH any buttonbar panel if active before switching to splitter
            if (buttonbarActive && !currentPanel.buttons.isEmpty()) {
                qDebug() << "[Parser] End of buttonbar panel, buffering panel with" << currentPanel.buttons.size() << "buttons.";
                printButtons(currentPanel.buttons, 1);
                buttonBarPanelsBuffer.append(currentPanel);
                buttonbarActive = false;
                currentPanel = CmdMasterPanel();
                currentButtonList = nullptr;
            }
            CmdMasterPanel splitterPanel;
            splitterPanel.type = "splitter";
            splitterPanel.attributes.clear();
            CmdMasterSplitter splitter;
            QRegularExpression orientationRe(R"(orientation:(\w+))");
            auto mo = orientationRe.match(line);
            if (mo.hasMatch()) {
                splitter.orientation = mo.captured(1);
                splitterPanel.attributes["orientation"] = splitter.orientation;
            }
            QRegularExpression sizesRe(R"s(sizes:([0-9, ]+))s");
            auto ms = sizesRe.match(line);
            if (ms.hasMatch()) {
                splitter.sizes = parseSizes(ms.captured(1));
            }
            splitterPanel.splitter = splitter;
            printSplitter(splitter, 1);
            splitterPanelsBuffer.append(splitterPanel);
            menubarActive = false;
            buttonbarActive = false;
            currentSection.clear();
            continue; // splitter has no children
        }
        // --- FoldableMenu (inside menubar) ---
        else if (line.startsWith("FoldableMenu")) {
            QRegularExpression re(R"(FoldableMenu\s*\"([^\"]+)\")");
            auto match = re.match(line);
            if (match.hasMatch()) {
                CmdMasterMenuItem currentMenu;
                currentMenu.label = match.captured(1);
                currentMenu.subItems.clear();
                if (currentMenuList) {
                    currentMenuList->append(currentMenu);
                    lastMenuIdx = currentMenuList->size() - 1;
                } else {
                    lastMenuIdx = -1;
                }
                qDebug() << "[Parser] New FoldableMenu:" << currentMenu.label;
            }
        }
        // --- Item (inside FoldableMenu) ---
        else if (line.startsWith("Item")) {
            QRegularExpression re(R"(Item\s*\"([^\"]+)\")");
            auto match = re.match(line);
            if (match.hasMatch()) {
                CmdMasterMenuItem item;
                item.label = match.captured(1);
                QRegularExpression reAction(R"(action:([a-zA-Z0-9_]+))");
                auto matchAction = reAction.match(line);
                if (matchAction.hasMatch()) item.action = matchAction.captured(1);
                QRegularExpression reShortcut(R"(shortcut:([^\s]+))");
                auto matchShortcut = reShortcut.match(line);
                if (matchShortcut.hasMatch()) item.shortcut = matchShortcut.captured(1);
                QRegularExpression reIcon(R"(icon:([^\s]+))");
                auto matchIcon = reIcon.match(line);
                if (matchIcon.hasMatch()) item.icon = matchIcon.captured(1);
                if (currentMenuList && lastMenuIdx >= 0 && lastMenuIdx < currentMenuList->size()) {
                    (*currentMenuList)[lastMenuIdx].subItems.append(item);
                    qDebug() << "[Parser] Added Item:" << item.label << "to FoldableMenu:" << (*currentMenuList)[lastMenuIdx].label;
                } else {
                    qWarning() << "[Parser] Item found but no valid FoldableMenu!";
                }
            }
        }
        // --- Separator (inside FoldableMenu) ---
        else if (line.startsWith("Separator")) {
            if (currentMenuList && lastMenuIdx >= 0 && lastMenuIdx < currentMenuList->size()) {
                CmdMasterMenuItem sep;
                sep.label = "---";
                (*currentMenuList)[lastMenuIdx].subItems.append(sep);
                qDebug() << "[Parser] Added Separator to FoldableMenu:" << (*currentMenuList)[lastMenuIdx].label;
            } else {
                qWarning() << "[Parser] Separator found but no valid FoldableMenu!";
            }
        }
        // --- Button (inside buttonbar) ---
        else if (line.startsWith("Button")) {
            QRegularExpression re(R"(Button\s*\"([^\"]+)\")");
            auto match = re.match(line);
            if (match.hasMatch()) {
                CmdMasterButton btn;
                btn.label = match.captured(1);
                QRegularExpression reAction(R"(action:([a-zA-Z0-9_]+))");
                auto matchAction = reAction.match(line);
                if (matchAction.hasMatch()) btn.action = matchAction.captured(1);
                QRegularExpression reIcon(R"(icon:([^\s]+))");
                auto matchIcon = reIcon.match(line);
                if (matchIcon.hasMatch()) btn.icon = matchIcon.captured(1);
                if (currentButtonList) {
                    currentButtonList->append(btn);
                    qDebug() << "[Parser] Added Button:" << btn.label << "to buttonbar";
                } else {
                    qWarning() << "[Parser] Button found but not in buttonbar!";
                }
            }
        }
        // --- Commands section ---
        else if (line.startsWith("Commands")) {
            // Flush menubar if active
            if (menubarActive && !currentPanel.menuItems.isEmpty()) {
                qDebug() << "[Parser] End of menubar panel, buffering panel with" << currentPanel.menuItems.size() << "menus.";
                printMenuItems(currentPanel.menuItems, 1);
                menuBarPanelsBuffer.append(currentPanel);
                menubarActive = false;
                currentPanel = CmdMasterPanel();
                currentMenuList = nullptr;
                lastMenuIdx = -1;
            }
            // Flush buttonbar if active
            if (buttonbarActive && !currentPanel.buttons.isEmpty()) {
                qDebug() << "[Parser] End of buttonbar panel, buffering panel with" << currentPanel.buttons.size() << "buttons.";
                printButtons(currentPanel.buttons, 1);
                buttonBarPanelsBuffer.append(currentPanel);
                buttonbarActive = false;
                currentPanel = CmdMasterPanel();
                currentButtonList = nullptr;
            }
            currentPanel = CmdMasterPanel();
            currentMenuList = nullptr;
            currentButtonList = nullptr;
            lastMenuIdx = -1;
            currentSection = "commands";
            qDebug() << "[Parser] Commands section started";
        }
        // --- Command block parsing ---
        else if (line.startsWith("Command")) {
            QRegularExpression re(R"(Command\s+([a-zA-Z0-9_]+))");
            auto match = re.match(line);
            if (match.hasMatch()) {
                CmdMasterCommand cmd;
                cmd.name = match.captured(1);
                qDebug() << "[Parser] New Command:" << cmd.name;
                // Read command block
                while (!in.atEnd()) {
                    QString cline = in.readLine().trimmed();
                    ++lineNumber;
                    if (cline.isEmpty()) {
                        qDebug() << "[Parser] End of command block for:" << cmd.name;
                        break;
                    }
                    qDebug() << "[Parser] Command block line:" << cline;
                    if (cline.startsWith("alias:")) {
                        cmd.alias = cline.mid(6).trimmed();
                        qDebug() << "[Parser]  alias:" << cmd.alias;
                    } else if (cline.startsWith("Description:")) {
                        cmd.description = cline.mid(12).trimmed();
                        qDebug() << "[Parser]  description:" << cmd.description;
                    } else if (cline.startsWith("Args:")) {
                        cmd.args = cline.mid(5).trimmed().split(' ', Qt::SkipEmptyParts);
                        qDebug() << "[Parser]  args:" << cmd.args;
                    } else if (cline.startsWith("Out:")) {
                        cmd.out = cline.mid(4).trimmed();
                        qDebug() << "[Parser]  out:" << cmd.out;
                    } else if (cline.startsWith("Shell:")) {
                        cmd.shell = cline.mid(6).trimmed();
                        qDebug() << "[Parser]  shell:" << cmd.shell;
                    } else {
                        qDebug() << "[Parser]  Unknown command block line:" << cline;
                        break;
                    }
                }
                commandsBuffer.insert(cmd.name, cmd);
            }
        }
    }

    // --- Final flush for any open panels ---
    if (menubarActive && !currentPanel.menuItems.isEmpty()) {
        qDebug() << "[Parser] End of file, buffering last menubar panel with" << currentPanel.menuItems.size() << "menus.";
        printMenuItems(currentPanel.menuItems, 1);
        menuBarPanelsBuffer.append(currentPanel);
    }
    if (buttonbarActive && !currentPanel.buttons.isEmpty()) {
        qDebug() << "[Parser] End of file, buffering last buttonbar panel with" << currentPanel.buttons.size() << "buttons.";
        printButtons(currentPanel.buttons, 1);
        buttonBarPanelsBuffer.append(currentPanel);
    }

    // Copy parsed buffers to config
    outConfig.menuBarPanels = menuBarPanelsBuffer;
    outConfig.buttonBarPanels = buttonBarPanelsBuffer;
    outConfig.splitterPanels = splitterPanelsBuffer;
    outConfig.commands = commandsBuffer;

    // Debug print all
    qDebug() << "[Parser] MenuBarPanels parsed:" << outConfig.menuBarPanels.size();
    for (int i = 0; i < outConfig.menuBarPanels.size(); ++i) {
        qDebug() << "[Parser] Panel" << i << "type:" << outConfig.menuBarPanels[i].type
                 << "menuItems:" << outConfig.menuBarPanels[i].menuItems.size();
        printMenuItems(outConfig.menuBarPanels[i].menuItems, 2);
    }
    qDebug() << "[Parser] ButtonBarPanels parsed:" << outConfig.buttonBarPanels.size();
    for (int i = 0; i < outConfig.buttonBarPanels.size(); ++i) {
        qDebug() << "[Parser] Panel" << i << "type:" << outConfig.buttonBarPanels[i].type
                 << "buttons:" << outConfig.buttonBarPanels[i].buttons.size();
        printButtons(outConfig.buttonBarPanels[i].buttons, 2);
    }
    qDebug() << "[Parser] SplitterPanels parsed:" << outConfig.splitterPanels.size();
    for (int i = 0; i < outConfig.splitterPanels.size(); ++i) {
        qDebug() << "[Parser] Panel" << i << "type:" << outConfig.splitterPanels[i].type;
        printSplitter(outConfig.splitterPanels[i].splitter, 2);
    }
    qDebug() << "[Parser] Commands parsed:" << outConfig.commands.size();
    for (auto it = outConfig.commands.begin(); it != outConfig.commands.end(); ++it) {
        qDebug() << "[Parser] Command:" << it.key()
                 << "alias:" << it.value().alias
                 << "desc:" << it.value().description
                 << "args:" << it.value().args
                 << "out:" << it.value().out
                 << "shell:" << it.value().shell;
    }

    return true;
}
