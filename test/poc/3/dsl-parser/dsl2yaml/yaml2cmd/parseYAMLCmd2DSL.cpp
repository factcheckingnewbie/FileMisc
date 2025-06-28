#include "parseYAMLCmd2DSL.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <yaml-cpp/yaml.h>

// Helper: Remove surrounding double-quotes if present
static QString unquote(const QString &str) {
    if (str.startsWith('\"') && str.endsWith('\"') && str.size() > 1)
        return str.mid(1, str.size() - 2);
    return str;
}

bool parseYAMLCmd2DSL(const QString &yamlPath, const QString &dslPath) {
    qDebug() << "[parseYAMLCmd2DSL] Opening YAML:" << yamlPath;
    QFile file(yamlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[parseYAMLCmd2DSL] Failed to open YAML file:" << yamlPath;
        return false;
    }
    QByteArray yamlContent = file.readAll();
    file.close();

    YAML::Node root;
    try {
        root = YAML::Load(yamlContent.constData());
    } catch (const YAML::ParserException &e) {
        qWarning() << "[parseYAMLCmd2DSL] YAML parse error:" << e.what();
        return false;
    }

    QFile dslFile(dslPath);
    if (!dslFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[parseYAMLCmd2DSL] Failed to open DSL output:" << dslPath;
        return false;
    }
    QTextStream out(&dslFile);

    // Begin DSL
    out << "Commands\n";

    if (!root["commands"] || !root["commands"].IsSequence()) {
        qWarning() << "[parseYAMLCmd2DSL] No commands group found in YAML";
        dslFile.close();
        return false;
    }

    for (const auto &groupNode : root["commands"]) {
        QString groupName;
        if (groupNode["group"])
            groupName = QString::fromStdString(groupNode["group"].as<std::string>());
        else
            groupName = "UnnamedGroup";
        out << "Group " << groupName << '\n';

        if (!groupNode["commands"] || !groupNode["commands"].IsSequence()) continue;
        for (const auto &cmdNode : groupNode["commands"]) {
            QString cmdName;
            if (cmdNode["name"])
                cmdName = QString::fromStdString(cmdNode["name"].as<std::string>());
            else
                continue; // skip if no name

            // Inline alias if present
            if (cmdNode["alias"]) {
                QString alias = QString::fromStdString(cmdNode["alias"].as<std::string>());
                out << "Command " << cmdName << " alias:" << alias << "\n";
            } else {
                out << "Command " << cmdName << "\n";
            }

            // Description
            if (cmdNode["description"]) {
                QString desc = QString::fromStdString(cmdNode["description"].as<std::string>());
                out << "Description: " << unquote(desc) << "\n";
            }
            // Args
            if (cmdNode["args"] && cmdNode["args"].IsSequence()) {
                out << "Args:";
                for (const auto &argNode : cmdNode["args"]) {
                    QString arg = QString::fromStdString(argNode.as<std::string>());
                    out << " " << arg;
                }
                out << "\n";
            }
            // Out
            if (cmdNode["out"]) {
                QString outval = QString::fromStdString(cmdNode["out"].as<std::string>());
                out << "Out: " << outval << "\n";
            }
            // Shell
            if (cmdNode["shell"]) {
                QString shell = QString::fromStdString(cmdNode["shell"].as<std::string>());
                out << "Shell: " << unquote(shell) << "\n";
            }
            out << "\n";
        }
    }

    dslFile.close();
    qDebug() << "[parseYAMLCmd2DSL] DSL written to:" << dslPath;
    return true;
}
