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
    QString input = argc > 1 ? argv[1] : "CommandMasterAppMenu.dsl";
    QString output = argc > 2 ? argv[2] : "menu.yaml";

    qDebug() << "Parsing DSL file:" << input;
    qDebug() << "Writing YAML output to:" << output;

    bool ok = parseDSLMenuToYaml(input, output);

    if (ok) {
        qDebug() << "SUCCESS: YAML written to" << output;
        printFile(output);
        return 0;
    } else {
        qDebug() << "FAIL: Parsing or YAML writing failed!";
        return 1;
    }
}
