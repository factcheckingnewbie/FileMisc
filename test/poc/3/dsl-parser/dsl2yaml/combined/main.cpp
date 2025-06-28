#include "parseDSLCmd2yaml.h"
#include "parseDSLMenu2yaml.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>

// Helper to print YAML output file to stdout (for test verification)
void printFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open result YAML for printing:" << filename;
        return;
    }
    QTextStream in(&f);
    qDebug().noquote() << "----- YAML OUTPUT (" << filename << ") -----";
    while (!in.atEnd()) {
        qDebug().noquote() << in.readLine();
    }
    qDebug().noquote() << "----- END YAML OUTPUT -----";
    f.close();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Accept input and output file from command line, or use defaults
    QString inputMenu = argc > 1 ? argv[1] : "CommandMasterAppMenu.dsl";
    QString outputMenu = argc > 2 ? argv[2] : "menu.yaml";

    // Accept input and output file from command line, or use defaults
    QString inputCmd = argc > 1 ? argv[1] : "CommandMasterAppCmd.dsl";
    QString outputCmd = argc > 2 ? argv[2] : "cmd.yaml";

    qDebug() << "Parsing Menu DSL file:" << inputMenu;
    qDebug() << "Writing Menu YAML output to:" << outputMenu;

    qDebug() << "Parsing Cmd DSL file:" << inputCmd;
    qDebug() << "Writing Cmd YAML output to:" << outputCmd;

    bool okMenu = parseDSLMenuToYaml(inputMenu, outputMenu);
    bool okCmd = parseDSLCommandsToYaml(inputCmd, outputCmd);

    if (okMenu) {
        qDebug() << "SUCCESS: Menu YAML written to" << outputMenu;
        printFile(outputMenu);
        return 0;
    } else {
        qDebug() << "FAIL: Parsing or Menu YAML writing failed!";
        return 1;
    }
    if (okCmd) {
        qDebug() << "SUCCESS: Menu YAML written to" << outputCmd;
        printFile(outputCmd);
        return 0;
    } else {
        qDebug() << "FAIL: Parsing or Cmd YAML writing failed!";
        return 1;
    }
}
