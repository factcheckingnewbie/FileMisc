#include "parseYAMLCmd2DSL.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>

// Helper to print DSL output file to stdout (for test verification)
void printFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open result DSL for printing:" << filename;
        return;
    }
    QTextStream in(&f);
    qDebug().noquote() << "----- DSL OUTPUT (" << filename << ") -----";
    while (!in.atEnd()) {
        qDebug().noquote() << in.readLine();
    }
    qDebug().noquote() << "----- END DSL OUTPUT -----";
    f.close();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Accept input and output file from command line, or use defaults
    QString input = argc > 1 ? argv[1] : "command.yaml";
    QString output = argc > 2 ? argv[2] : "CommandMasterAppCmd.dsl";

    qDebug() << "Parsing YAML file:" << input;
    qDebug() << "Writing DSL output to:" << output;

    bool ok = parseYAMLCmd2DSL(input, output);

    if (ok) {
        qDebug() << "SUCCESS: DSL written to" << output;
        printFile(output);
        return 0;
    } else {
        qDebug() << "FAIL: Parsing or DSL writing failed!";
        return 1;
    }
}
