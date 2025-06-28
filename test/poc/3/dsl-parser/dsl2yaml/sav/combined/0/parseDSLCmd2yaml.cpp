     // #include "parseDSLCmd2yaml.h"
#include "parseDSL2yaml.h"
 
#include <QFile>
 #include <QTextStream>
 #include <QRegularExpression>
 #include <QDebug>
 
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
     // FIX: .replace() on const QString - make a mutable copy
     out << ind << key << ": \"" << QString(val).replace("\"", "\\\"") << "\"\n";
 }
 
 static void writeYamlList(QTextStream &out, const QString &key, const QStringList &vals, int indent = 0) {
     QString ind(indent, ' ');
     out << ind << key << ":\n";
     for (const auto &v : vals) {
         // FIX: .replace() on const QString - make a mutable copy
         out << ind << "  - \"" << QString(v).replace("\"", "\\\"") << "\"\n";
     }
 }
 
 // --- DSL Command Section Parser and YAML Writer ---
 bool parseDSLCommandsToYaml(const QString &filePath, const QString &yamlPath) {
     qDebug() << "[parseDSLCommandsToYaml] Opening file:" << filePath;
     QFile file(filePath);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
         qWarning() << "[parseDSLCommandsToYaml] Failed to open file:" << filePath;
         return false;
     }
     QTextStream in(&file);
 
     QFile yamlFile(yamlPath);
     if (!yamlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
         qWarning() << "[parseDSLCommandsToYaml] Failed to open YAML output:" << yamlPath;
         return false;
     }
     QTextStream out(&yamlFile);
 
     QString currentGroupLabel;
     bool insideCommands = false;
     bool insideGroup = false;
     bool insidePipeline = false;
     QString pipelineName;
     // QList<QString> pipelineSteps; // Commented out: not used in logic, can add later if pipeline steps stored
 
     // --- NB: Only one pipeline is supported in first version.
     // If multiple pipelines in DSL, only last one will be written to YAML.
     // If supporting multiple pipelines is needed, refactor after first test version.
     // Never trust the future, fix when extending pipeline support.
 
     out << "commands:\n";
     int lineNumber = 0;
 
     while (!in.atEnd()) {
         QString rawLine = in.readLine();
         ++lineNumber;
         QString line = rawLine.trimmed();
         // int indent = getIndent(rawLine); // not used, see above
 
         qDebug() << "[YAMLParser] Line" << lineNumber << ":" << rawLine;
 
         if (line.isEmpty() || line.startsWith('#')) {
             qDebug() << "[YAMLParser] Skipping empty or comment line.";
             continue;
         }
 
         // --- Commands section start ---
         if (line.startsWith("Commands")) {
             insideCommands = true;
             qDebug() << "[YAMLParser] Commands section started.";
             continue;
         }
 
         // --- Group start ---
         if (line.startsWith("Group")) {
             QRegularExpression re(R"(Group\s+([^\s]+)(?:\s+label:\"([^\"]+)\")?)");
             auto match = re.match(line);
             if (match.hasMatch()) {
                 currentGroupLabel = match.captured(2).isEmpty() ? match.captured(1) : match.captured(2);
                 out << "  - group: \"" << QString(currentGroupLabel).replace("\"", "\\\"") << "\"\n";
                 out << "    commands:\n";
                 insideGroup = true;
                 qDebug() << "[YAMLParser] Group started:" << currentGroupLabel;
                 // NOTE: There is no explicit group close; all commands until next group are in same group.
                 // This will accumulate all commands in one group if not careful.
                 // Must refactor after first test version for explicit group scoping.
                 // Never trust the future, always fix this when extending.
             }
             continue;
         }

        // --- Command block ---
        if (line.startsWith("Command")) {
            QRegularExpression re(R"(Command\s+([a-zA-Z0-9_]+)(?:\s+alias:([a-zA-Z0-9_]+))?)");
            auto match = re.match(line);
            if (match.hasMatch()) {
                QString cmdName = match.captured(1);
                QString alias;
                // Parse possible inline alias
                if (match.captured(2).isEmpty()) {
                    alias.clear();
                } else {
                    alias = match.captured(2);
                }
                out << "      - name: \"" << QString(cmdName).replace("\"", "\\\"") << "\"\n";
                if (!alias.isEmpty())
                    out << "        alias: \"" << QString(alias).replace("\"", "\\\"") << "\"\n";
                QString description, outVal, shell;
                QStringList args;
                // Parse command block
                while (!in.atEnd()) {
                    QString cline = in.readLine();
                    ++lineNumber;
                    QString ctrim = cline.trimmed();
                    if (ctrim.isEmpty()) break;
                    qDebug() << "[YAMLParser] Command block line:" << ctrim;
                    if (ctrim.startsWith("alias:")) {
                        // NOTE: Alias is handled both inline and as block, redundant.
                        // After first test version, refactor to allow only one way (prefer block for clarity).
                        // Redundant alias handling is a code confusion risk.
                        alias = ctrim.mid(6).trimmed();
                        out << "        alias: \"" << QString(alias).replace("\"", "\\\"") << "\"\n";
                    } else if (ctrim.startsWith("Description:")) {
                        description = ctrim.mid(12).trimmed();
                        writeYamlString(out, "description", description, 8);
                    } else if (ctrim.startsWith("Args:")) {
                        QString argStr = ctrim.mid(5).trimmed();
                        // NOTE: Arguments are split by space, so any arg with a space will break.
                        // For now, stick to single-word args or use a delimiter.
                        // When refactoring, parse arguments more robustly.
                        args = argStr.split(' ', Qt::SkipEmptyParts);
                        if (!args.isEmpty()) {
                            out << "        args:\n";
                            for (const auto &a : args)
                                // FIX: .replace() on const QString - make a mutable copy
                                out << "          - \"" << QString(a).replace("\"", "\\\"") << "\"\n";
                        }
                    } else if (ctrim.startsWith("Out:")) {
                        outVal = ctrim.mid(4).trimmed();
                        writeYamlString(out, "out", outVal, 8);
                    } else if (ctrim.startsWith("Shell:")) {
                        shell = ctrim.mid(6).trimmed();
                        writeYamlString(out, "shell", shell, 8);
                    } else {
                        qDebug() << "[YAMLParser] Unknown command block line:" << ctrim;
                        break;
                    }
                }
            }
            continue;
        }

        // --- Pipeline block ---
        // --- Safe as long we stay out of async mode ---
        if (line.startsWith("Pipeline")) {
            QRegularExpression re(R"(Pipeline\s*\"([^\"]+)\")");
            auto match = re.match(line);
            if (match.hasMatch()) {
                pipelineName = match.captured(1);
                out << "pipeline:\n";
                out << "  name: \"" << QString(pipelineName).replace("\"", "\\\"") << "\"\n";
                out << "  steps:\n";
                insidePipeline = true;
                // pipelineSteps.clear(); // Commented out: not used in logic, can add later if storing steps
                qDebug() << "[YAMLParser] Pipeline started:" << pipelineName;
            }
            continue;
        }
        if (insidePipeline && line.startsWith("Step")) {
            QRegularExpression re(R"(Step\s+([a-zA-Z0-9_]+)\s*(.*))");
            auto match = re.match(line);
            if (match.hasMatch()) {
                QString stepName = match.captured(1);
                QString params = match.captured(2).trimmed();
                out << "    - name: \"" << QString(stepName).replace("\"", "\\\"") << "\"\n";
                if (!params.isEmpty()) {
                    // parse param:value pairs
                    QRegularExpression paramRe(R"(([a-zA-Z_]+):\"([^\"]+)\")");
                    auto it = paramRe.globalMatch(params);
                    while (it.hasNext()) {
                        auto m = it.next();
                        out << "      " << QString(m.captured(1)).replace("\"", "\\\"") << ": \"" 
                            << QString(m.captured(2)).replace("\"", "\\\"") << "\"\n";
                    }
                }
            }
            continue;
        }
        if (insidePipeline && !line.startsWith("Step")) {
            insidePipeline = false;
            pipelineName.clear();
            // pipelineSteps.clear(); // Commented out: see above
            qDebug() << "[YAMLParser] Pipeline ended.";
        }
    }

    yamlFile.close();
    file.close();
    qDebug() << "[parseDSLCommandsToYaml] YAML written to:" << yamlPath;
    return true;
 }
