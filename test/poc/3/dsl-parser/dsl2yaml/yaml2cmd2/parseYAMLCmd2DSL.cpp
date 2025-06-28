#include "parseYAMLCmd2DSL.h" 
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <yaml-cpp/yaml.h>

// Helper: Write N blank lines
static void writeBlankLines(QTextStream &out, int n) {
    for (int i = 0; i < n; ++i) out << "\n";
}

// Helper: Write indentation (spaces)
static void writeIndent(QTextStream &out, int indent) {
    for (int i = 0; i < indent; ++i) out << " ";
}

// Helper: Write quoted or unquoted string, NO escaping of quotes or backslashes
static void writeMaybeQuoted(QTextStream &out, const QString &val, bool quoted) {
    if (quoted)
        out << "\"" << val << "\"";
    else
        out << val;
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

    // Groups
    if (!root["commands"] || !root["commands"].IsSequence()) {
        qWarning() << "[parseYAMLCmd2DSL] No commands group found in YAML";
        dslFile.close();
        return false;
    }

    for (const auto &groupNode : root["commands"]) {
        int groupIndent = groupNode["indent"] ? groupNode["indent"].as<int>() : 0;
        int groupBLBefore = groupNode["blank_lines_before"] ? groupNode["blank_lines_before"].as<int>() : 0;
        int groupBLAfter = groupNode["blank_lines_after"] ? groupNode["blank_lines_after"].as<int>() : 0;

        writeBlankLines(out, groupBLBefore);
        writeIndent(out, groupIndent);

        QString groupName = groupNode["group_name"] ? QString::fromStdString(groupNode["group_name"].as<std::string>()) : "UnknownGroup";
        QString groupLabel = groupNode["group_label"] ? QString::fromStdString(groupNode["group_label"].as<std::string>()) : "";
        out << "Group " << groupName;
        if (!groupLabel.isEmpty())
            out << " label:\"" << groupLabel << "\"";
        out << "\n";

        // Commands
        if (!groupNode["commands"] || !groupNode["commands"].IsSequence()) continue;
        for (const auto &cmdNode : groupNode["commands"]) {
            int cmdIndent = cmdNode["indent"] ? cmdNode["indent"].as<int>() : 0;
            int cmdBLBefore = cmdNode["blank_lines_before"] ? cmdNode["blank_lines_before"].as<int>() : 0;
            int cmdBLAfter = cmdNode["blank_lines_after"] ? cmdNode["blank_lines_after"].as<int>() : 0;
            writeBlankLines(out, cmdBLBefore);
            writeIndent(out, cmdIndent);

            QString cmdName = cmdNode["name"] ? QString::fromStdString(cmdNode["name"].as<std::string>()) : "";
            QString cmdAlias = cmdNode["alias"] ? QString::fromStdString(cmdNode["alias"].as<std::string>()) : "";
            out << "Command " << cmdName;
            if (!cmdAlias.isEmpty())
                out << " alias:" << cmdAlias;
            out << "\n";

            QString fieldIndentStr = QString("").fill(' ', cmdIndent+4);
            // Description
            if (cmdNode["description"]) {
                QString desc = QString::fromStdString(cmdNode["description"].as<std::string>());
                bool descQuoted = cmdNode["description_quoted"] && cmdNode["description_quoted"].as<bool>();
                out << fieldIndentStr << "Description: ";
                writeMaybeQuoted(out, desc, descQuoted);
                out << "\n";
            }
            // Args
            if (cmdNode["args"] && cmdNode["args"].IsSequence()) {
                out << fieldIndentStr << "Args:";
                for (const auto &argNode : cmdNode["args"]) {
                    QString arg = QString::fromStdString(argNode.as<std::string>());
                    out << " " << arg;
                }
                out << "\n";
            }
            // Out
            if (cmdNode["out"]) {
                QString outval = QString::fromStdString(cmdNode["out"].as<std::string>());
                out << fieldIndentStr << "Out: " << outval << "\n";
            }
            // Shell
            if (cmdNode["shell"]) {
                QString shell = QString::fromStdString(cmdNode["shell"].as<std::string>());
                bool shellQuoted = cmdNode["shell_quoted"] && cmdNode["shell_quoted"].as<bool>();
                out << fieldIndentStr << "Shell: ";
                writeMaybeQuoted(out, shell, shellQuoted);
                out << "\n";
            }
            writeBlankLines(out, cmdBLAfter);
        }
        writeBlankLines(out, groupBLAfter);
    }

    // Pipeline
    if (root["pipeline"]) {
        auto p = root["pipeline"];
        int pipeIndent = p["indent"] ? p["indent"].as<int>() : 0;
        int pipeBLBefore = p["blank_lines_before"] ? p["blank_lines_before"].as<int>() : 0;
        int pipeBLAfter = p["blank_lines_after"] ? p["blank_lines_after"].as<int>() : 0;
        writeBlankLines(out, pipeBLBefore);
        writeIndent(out, pipeIndent);
        out << "Pipeline \"" << QString::fromStdString(p["name"].as<std::string>()) << "\"\n";
        // Steps
        if (p["steps"] && p["steps"].IsSequence()) {
            for (const auto &step : p["steps"]) {
                int sIndent = step["indent"] ? step["indent"].as<int>() : 0;
                writeIndent(out, sIndent);
                out << "Step " << QString::fromStdString(step["name"].as<std::string>());
                // Output args
                for (auto it = step.begin(); it != step.end(); ++it) {
                    std::string k = it->first.as<std::string>();
                    if (k == "name" || k == "indent") continue;
                    QString v = QString::fromStdString(it->second.as<std::string>());
                    out << " " << QString::fromStdString(k) << ":";
                    // Only quote if value contains space or is empty
                    if (v.contains(' ') || v.isEmpty())
                        out << "\"" << v << "\"";
                    else
                        out << v;
                }
                out << "\n";
            }
        }
        writeBlankLines(out, pipeBLAfter);
    }

    dslFile.close();
    qDebug() << "[parseYAMLCmd2DSL] DSL written to:" << dslPath;
    return true;
}
