#include "parseDSLMenu2yaml.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QVariant>

// --- Helpers ---
// getIndent is not used in current version, but can be useful for tab/dots/tree indentation parsing in future.
// static int getIndent(const QString &line) {
//     int i = 0;
//     while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
//     return i;
// }

// --- YAML Writer Helper ---
static void writeYamlString(QTextStream &out, const QString &key, const QString &val, int indent = 0) {
    QString ind(indent, ' ');
    out << ind << key << ": \"" << QString(val).replace("\"", "\\\"") << "\"\n";
}

// --- DSL Menu Section Parser and YAML Writer ---
bool parseDSLMenuToYaml(const QString &filePath, const QString &yamlPath) {
    qDebug() << "[parseDSLMenuToYaml] Opening file:" << filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[parseDSLMenuToYaml] Failed to open file:" << filePath;
        return false;
    }
    QTextStream in(&file);

    QFile yamlFile(yamlPath);
    if (!yamlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[parseDSLMenuToYaml] Failed to open YAML output:" << yamlPath;
        return false;
    }
    QTextStream out(&yamlFile);

    QString windowTitle;
    int windowWidth = 0, windowHeight = 0;
    bool insidePanel = false;
    QString panelType;
    QMap<QString, QString> panelAttrs;
    QList<QVariantMap> menuBar;
    QVariantMap currentMenu;
    QList<QVariantMap> currentItems;
    bool insideFoldableMenu = false;
    QString currentFoldableMenuLabel;
    int lineNumber = 0;

    while (!in.atEnd()) {
        QString rawLine = in.readLine();
        ++lineNumber;
        QString line = rawLine.trimmed();

        qDebug() << "[YAMLMenuParser] Line" << lineNumber << ":" << rawLine;

        if (line.isEmpty() || line.startsWith('#')) {
            qDebug() << "[YAMLMenuParser] Skipping empty or comment line.";
            continue;
        }

        // --- Window ---
        if (line.startsWith("Window")) {
            QRegularExpression re(R"(Window\s*\"([^\"]+)\")");
            auto match = re.match(line);
            if (match.hasMatch()) {
                windowTitle = match.captured(1);
                qDebug() << "[YAMLMenuParser] Parsed window title:" << windowTitle;
            }
            QRegularExpression reDim(R"(width:(\d+)\s*height:(\d+))");
            auto m2 = reDim.match(line);
            if (m2.hasMatch()) {
                windowWidth = m2.captured(1).toInt();
                windowHeight = m2.captured(2).toInt();
                qDebug() << "[YAMLMenuParser] Parsed window dimensions:" << windowWidth << "x" << windowHeight;
            }
            continue;
        }

        // --- Panel menubar ---
        if (line.startsWith("Panel menubar")) {
            // Flush last FoldableMenu if any
            if (insideFoldableMenu && !currentFoldableMenuLabel.isEmpty()) {
                currentMenu["label"] = currentFoldableMenuLabel;
                QVariantList variantItems;
                for (const auto& ci : currentItems) variantItems.append(ci);
                currentMenu["items"] = variantItems;
                menuBar.append(currentMenu);
                currentMenu.clear();
                currentItems.clear();
                currentFoldableMenuLabel.clear();
            }
            insideFoldableMenu = false;

            panelType = "menubar";
            panelAttrs.clear();
            QRegularExpression attrRe(R"((\w+):([^\s]+))");
            auto it = attrRe.globalMatch(line);
            while (it.hasNext()) {
                auto m = it.next();
                panelAttrs[m.captured(1)] = m.captured(2);
            }
            insidePanel = true;
            qDebug() << "[YAMLMenuParser] Panel menubar started";
            continue;
        }

        // --- Panel splitter (end menubar parsing, flush menu) ---
        if (line.startsWith("Panel splitter")) {
            if (insideFoldableMenu && !currentFoldableMenuLabel.isEmpty()) {
                currentMenu["label"] = currentFoldableMenuLabel;
                QVariantList variantItems;
                for (const auto& ci : currentItems) variantItems.append(ci);
                currentMenu["items"] = variantItems;
                menuBar.append(currentMenu);
                currentMenu.clear();
                currentItems.clear();
                currentFoldableMenuLabel.clear();
            }
            insideFoldableMenu = false;
            insidePanel = false;
            continue;
        }

        // --- Panel buttonbar (end menubar parsing, flush menu) ---
        if (line.startsWith("Panel buttonbar")) {
            if (insideFoldableMenu && !currentFoldableMenuLabel.isEmpty()) {
                currentMenu["label"] = currentFoldableMenuLabel;
                QVariantList variantItems;
                for (const auto& ci : currentItems) variantItems.append(ci);
                currentMenu["items"] = variantItems;
                menuBar.append(currentMenu);
                currentMenu.clear();
                currentItems.clear();
                currentFoldableMenuLabel.clear();
            }
            insideFoldableMenu = false;
            insidePanel = false;
            continue;
        }

        // --- FoldableMenu (inside menubar) ---
        if (line.startsWith("FoldableMenu")) {
            if (insideFoldableMenu && !currentFoldableMenuLabel.isEmpty()) {
                currentMenu["label"] = currentFoldableMenuLabel;
                QVariantList variantItems;
                for (const auto& ci : currentItems) variantItems.append(ci);
                currentMenu["items"] = variantItems;
                menuBar.append(currentMenu);
                currentMenu.clear();
                currentItems.clear();
            }
            QRegularExpression re(R"(FoldableMenu\s*\"([^\"]+)\")");
            auto match = re.match(line);
            if (match.hasMatch()) {
                currentFoldableMenuLabel = match.captured(1);
                insideFoldableMenu = true;
                qDebug() << "[YAMLMenuParser] New FoldableMenu:" << currentFoldableMenuLabel;
            }
            continue;
        }

        // --- Item (inside FoldableMenu) ---
        if (line.startsWith("Item")) {
            QRegularExpression re(R"(Item\s*\"([^\"]+)\")");
            auto match = re.match(line);
            QVariantMap item;
            if (match.hasMatch()) {
                item["label"] = match.captured(1);
                QRegularExpression reAction(R"(action:([a-zA-Z0-9_]+))");
                auto matchAction = reAction.match(line);
                if (matchAction.hasMatch()) item["action"] = matchAction.captured(1);
                QRegularExpression reShortcut(R"(shortcut:([^\s]+))");
                auto matchShortcut = reShortcut.match(line);
                if (matchShortcut.hasMatch()) item["shortcut"] = matchShortcut.captured(1);
                QRegularExpression reIcon(R"(icon:([^\s]+))");
                auto matchIcon = reIcon.match(line);
                if (matchIcon.hasMatch()) item["icon"] = matchIcon.captured(1);
                QRegularExpression reChecked(R"(checked:true)");
                auto matchChecked = reChecked.match(line);
                if (matchChecked.hasMatch()) item["checked"] = true;
                currentItems.append(item);
                qDebug() << "[YAMLMenuParser] Added Item:" << item;
            }
            continue;
        }

        // --- Separator (inside FoldableMenu) ---
        if (line.startsWith("Separator")) {
            QVariantMap sep;
            sep["separator"] = true;
            currentItems.append(sep);
            qDebug() << "[YAMLMenuParser] Added Separator";
            continue;
        }
    }

    // Final flush
    if (insideFoldableMenu && !currentFoldableMenuLabel.isEmpty()) {
        currentMenu["label"] = currentFoldableMenuLabel;
        QVariantList variantItems;
        for (const auto& ci : currentItems) variantItems.append(ci);
        currentMenu["items"] = variantItems;
        menuBar.append(currentMenu);
        currentMenu.clear();
        currentItems.clear();
        currentFoldableMenuLabel.clear();
    }

    // Write YAML output
    out << "window:\n";
    out << "  title: \"" << QString(windowTitle).replace("\"", "\\\"") << "\"\n";
    out << "  width: " << windowWidth << "\n";
    out << "  height: " << windowHeight << "\n";
    out << "menubar:\n";
    for (const auto &menu : menuBar) { // FIXED: menu is QVariantMap, not QVariant
        out << "  - label: \"" << QString(menu["label"].toString()).replace("\"", "\\\"") << "\"\n";
        out << "    items:\n";
        QVariantList items = menu["items"].toList();
        for (const auto &itemVar : items) {
            QVariantMap item = itemVar.toMap();
            if (item.contains("separator")) {
                out << "      - separator: true\n";
                continue;
            }
            out << "      - label: \"" << QString(item["label"].toString()).replace("\"", "\\\"") << "\"\n";
            if (item.contains("action"))
                out << "        action: \"" << QString(item["action"].toString()).replace("\"", "\\\"") << "\"\n";
            if (item.contains("shortcut"))
                out << "        shortcut: \"" << QString(item["shortcut"].toString()).replace("\"", "\\\"") << "\"\n";
            if (item.contains("icon"))
                out << "        icon: \"" << QString(item["icon"].toString()).replace("\"", "\\\"") << "\"\n";
            if (item.contains("checked"))
                out << "        checked: true\n";
        }
    }

    yamlFile.close();
    file.close();
    qDebug() << "[parseDSLMenuToYaml] YAML written to:" << yamlPath;
    return true;
}
