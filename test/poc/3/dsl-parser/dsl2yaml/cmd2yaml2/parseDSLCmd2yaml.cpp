#include "parseDSLCmd2yaml.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QVector>
#include <QStringList>
#include <QMap>

// Helper: Escape YAML string
static QString yamlEscape(const QString &s) {
    QString t = s;
    t.replace("\\", "\\\\").replace("\"", "\\\"");
    return "\"" + t + "\"";
}

// Helper: Count indent (spaces/tabs)
static int getIndent(const QString &line) {
    int i = 0;
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
    return i;
}

// Helper: Check if a string is quoted in DSL
static bool isQuoted(const QString &s) {
    QString t = s.trimmed();
    return t.startsWith("\"") && t.endsWith("\"") && t.size() >= 2;
}

// Helper: Remove quotes if present
static QString stripQuotes(const QString &s) {
    QString t = s.trimmed();
    if (t.startsWith("\"") && t.endsWith("\"") && t.size() >= 2)
        return t.mid(1, t.size() - 2);
    return t;
}

// Helper: Parse step arguments with order and quoting preserved
struct StepArg {
    QString key;
    QString value;
    bool quoted;
};

static QVector<StepArg> parseStepArgs(const QString& params) {
    QVector<StepArg> result;
    int pos = 0;
    QString p = params;
    while (pos < p.length()) {
        // skip whitespace
        while (pos < p.length() && p[pos].isSpace()) ++pos;
        if (pos >= p.length()) break;

        // parse key
        int keyStart = pos;
        while (pos < p.length() && ((p[pos].isLetterOrNumber()) || p[pos] == '_')) ++pos;
        if (pos == keyStart) break; // no key found
        QString key = p.mid(keyStart, pos - keyStart);

        // colon
        if (pos >= p.length() || p[pos] != ':') break;
        ++pos;

        // value
        QString value;
        bool quoted = false;
        if (pos < p.length() && p[pos] == '"') {
            quoted = true;
            ++pos;
            int valStart = pos;
            bool foundEnd = false;
            while (pos < p.length()) {
                if (p[pos] == '"' && (pos == valStart || p[pos-1] != '\\')) {
                    foundEnd = true;
                    break;
                }
                ++pos;
            }
            value = p.mid(valStart, pos - valStart);
            if (foundEnd) ++pos;
            value.replace("\\\"", "\"");
        } else {
            int valStart = pos;
            while (pos < p.length() && !p[pos].isSpace()) ++pos;
            value = p.mid(valStart, pos - valStart);
        }
        result.push_back({key, value, quoted});
    }
    return result;
}

// Main converter function with order and quoting preserved for step arguments
bool parseDSLCommandsToYaml(const QString &filePath, const QString &yamlPath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[parseDSLCmd2yaml] Failed to open DSL file:" << filePath;
        return false;
    }
    QTextStream in(&file);

    QFile yamlFile(yamlPath);
    if (!yamlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "[parseDSLCmd2yaml] Failed to open YAML output:" << yamlPath;
        return false;
    }
    QTextStream out(&yamlFile);

    QVector<QString> yamlLines; // Not used, can be used for post-processing if needed
    QVector<QString> dslLines;
    int lineNumber = 0;
    bool inCommands = false, inGroup = false, inPipeline = false;
    QString currentGroupName, currentGroupLabel;
    int currentGroupIndent = 0;
    int currentPipelineIndent = 0;
    int blankLinesPending = 0;

    struct CommandYaml {
        QString name;
        QString alias;
        int indent = 0;
        QString description;
        bool description_quoted = false;
        QStringList args;
        QString out;
        QString shell;
        bool shell_quoted = false;
        int blank_lines_before = 0;
        int blank_lines_after = 0;
    };
    struct GroupYaml {
        QString name;
        QString label;
        int indent = 0;
        QVector<CommandYaml> commands;
        int blank_lines_before = 0;
        int blank_lines_after = 0;
    };
    struct StepYaml {
        QString name;
        int indent = 0;
        QVector<StepArg> args;
    };
    struct PipelineYaml {
        QString name;
        int indent = 0;
        QVector<StepYaml> steps;
        int blank_lines_before = 0;
        int blank_lines_after = 0;
    };

    QVector<GroupYaml> groups;
    PipelineYaml pipeline;

    GroupYaml currentGroup;
    CommandYaml currentCmd;

    while (!in.atEnd()) {
        QString rawLine = in.readLine();
        dslLines.push_back(rawLine);
        ++lineNumber;

        if (rawLine.trimmed().isEmpty()) {
            ++blankLinesPending;
            continue;
        }

        QString line = rawLine.trimmed();
        int indent = getIndent(rawLine);

        if (line == "Commands") {
            inCommands = true;
            blankLinesPending = 0;
            continue;
        }

        if (line.startsWith("Group ")) {
            if (inGroup) {
                currentGroup.blank_lines_after = blankLinesPending;
                groups.push_back(currentGroup);
            }
            currentGroup = GroupYaml();
            currentGroup.indent = indent;
            currentGroup.blank_lines_before = blankLinesPending;
            currentGroup.commands.clear();
            blankLinesPending = 0;
            inGroup = true;
            QRegularExpression re(R"(^Group\s+([^\s]+)(?:\s+label:(\"[^\"]+\"))?$)");
            auto m = re.match(line);
            if (m.hasMatch()) {
                currentGroup.name = m.captured(1);
                currentGroup.label = m.captured(2);
                if (isQuoted(currentGroup.label)) currentGroup.label = stripQuotes(currentGroup.label);
            }
            continue;
        }

        if (line.startsWith("Command ")) {
            if (!currentCmd.name.isEmpty()) {
                currentCmd.blank_lines_after = blankLinesPending;
                currentGroup.commands.push_back(currentCmd);
            }
            currentCmd = CommandYaml();
            currentCmd.indent = indent;
            currentCmd.blank_lines_before = blankLinesPending;
            blankLinesPending = 0;
            QRegularExpression re(R"(^Command\s+([^\s]+)(?:\s+alias:([^\s]+))?)");
            auto m = re.match(line);
            if (m.hasMatch()) {
                currentCmd.name = m.captured(1);
                currentCmd.alias = m.captured(2);
            }
            continue;
        }

        if (line.startsWith("Description:")) {
            QString val = line.mid(QString("Description:").size()).trimmed();
            currentCmd.description = stripQuotes(val);
            currentCmd.description_quoted = isQuoted(val);
            continue;
        }
        if (line.startsWith("Args:")) {
            QString val = line.mid(QString("Args:").size()).trimmed();
            currentCmd.args = val.split(" ", Qt::SkipEmptyParts);
            continue;
        }
        if (line.startsWith("Out:")) {
            QString val = line.mid(QString("Out:").size()).trimmed();
            currentCmd.out = val;
            continue;
        }
        if (line.startsWith("Shell:")) {
            QString val = line.mid(QString("Shell:").size()).trimmed();
            currentCmd.shell = stripQuotes(val);
            currentCmd.shell_quoted = isQuoted(val);
            continue;
        }
        if (line.startsWith("Pipeline ")) {
            if (inGroup) {
                if (!currentCmd.name.isEmpty()) {
                    currentCmd.blank_lines_after = blankLinesPending;
                    currentGroup.commands.push_back(currentCmd);
                    currentCmd = CommandYaml();
                }
                currentGroup.blank_lines_after = blankLinesPending;
                groups.push_back(currentGroup);
                inGroup = false;
            }
            pipeline = PipelineYaml();
            pipeline.blank_lines_before = blankLinesPending;
            blankLinesPending = 0;
            inPipeline = true;
            pipeline.indent = indent;
            QRegularExpression re(R"(^Pipeline\s+\"([^\"]+)\")");
            auto m = re.match(line);
            if (m.hasMatch()) {
                pipeline.name = m.captured(1);
            }
            continue;
        }
        if (inPipeline && line.startsWith("Step ")) {
            StepYaml step;
            step.indent = indent;
            QRegularExpression re(R"(^Step\s+([^\s]+)\s*(.*)$)");
            auto m = re.match(line);
            if (m.hasMatch()) {
                step.name = m.captured(1);
                QString params = m.captured(2).trimmed();
                step.args = parseStepArgs(params);
            }
            pipeline.steps.push_back(step);
            continue;
        }
    }
    if (!currentCmd.name.isEmpty()) {
        currentCmd.blank_lines_after = blankLinesPending;
        currentGroup.commands.push_back(currentCmd);
    }
    if (inGroup) {
        currentGroup.blank_lines_after = blankLinesPending;
        groups.push_back(currentGroup);
    }
    if (inPipeline) {
        pipeline.blank_lines_after = blankLinesPending;
    }

    // Output YAML
    out << "commands:\n";
    for (const auto& group : groups) {
        out << "  - group_name: " << group.name << "\n";
        out << "    group_label: " << yamlEscape(group.label) << "\n";
        out << "    indent: " << group.indent << "\n";
        out << "    blank_lines_before: " << group.blank_lines_before << "\n";
        out << "    blank_lines_after: " << group.blank_lines_after << "\n";
        out << "    commands:\n";
        for (const auto& cmd : group.commands) {
            out << "      - name: " << cmd.name << "\n";
            out << "        alias: " << cmd.alias << "\n";
            out << "        indent: " << cmd.indent << "\n";
            out << "        description: " << yamlEscape(cmd.description) << "\n";
            out << "        description_quoted: " << (cmd.description_quoted ? "true" : "false") << "\n";
            out << "        args:\n";
            for (const auto& arg : cmd.args)
                out << "          - " << yamlEscape(arg) << "\n";
            out << "        out: " << yamlEscape(cmd.out) << "\n";
            out << "        shell: " << yamlEscape(cmd.shell) << "\n";
            out << "        shell_quoted: " << (cmd.shell_quoted ? "true" : "false") << "\n";
            out << "        blank_lines_before: " << cmd.blank_lines_before << "\n";
            out << "        blank_lines_after: " << cmd.blank_lines_after << "\n";
        }
    }
    if (!pipeline.name.isEmpty()) {
        out << "pipeline:\n";
        out << "  name: " << yamlEscape(pipeline.name) << "\n";
        out << "  indent: " << pipeline.indent << "\n";
        out << "  blank_lines_before: " << pipeline.blank_lines_before << "\n";
        out << "  blank_lines_after: " << pipeline.blank_lines_after << "\n";
        out << "  steps:\n";
        for (const auto& step : pipeline.steps) {
            out << "    - name: " << step.name << "\n";
            out << "      indent: " << step.indent << "\n";
            for (const auto& arg : step.args) {
                out << "      " << arg.key << ": " << yamlEscape(arg.value) << "\n";
                out << "      " << arg.key << "_quoted: " << (arg.quoted ? "true" : "false") << "\n";
            }
        }
    }
    out << "meta:\n";
    out << "  preserve_blank_lines: true\n";
    out << "  preserve_indentation: true\n";
    out << "  original_format: \"dsl\"\n";
    out << "  original_lines:\n";
    for (const auto& l : dslLines)
        out << "    - " << yamlEscape(l) << "\n";

    yamlFile.close();
    file.close();
    return true;
}
