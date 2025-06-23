#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QDockWidget>
#include <QDebug>
#include "DynamicUiFactory.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow mainWin;
    mainWin.setWindowTitle("Dynamic YAML Panel Demo");

    // Output area
    QTextEdit *output = new QTextEdit;
    output->setReadOnly(true);
    QDockWidget *dock = new QDockWidget("Command Output");
    dock->setWidget(output);
    mainWin.addDockWidget(Qt::BottomDockWidgetArea, dock);

    // Load panel from YAML
    DynamicUiFactory *factory = new DynamicUiFactory(&mainWin);
    QWidget *panel = factory->createPanelFromYaml("buttons.yaml");

    QObject::connect(factory, &DynamicUiFactory::commandExecuted, [&](const QString &cmd, int exitCode, const QString &out, const QString &err){
        output->append(QString("Command: %1\nExit: %2\nOutput:\n%3\nError:\n%4\n-----\n")
                        .arg(cmd).arg(exitCode).arg(out).arg(err));
    });

    mainWin.setCentralWidget(panel);
    mainWin.resize(400, 300);
    mainWin.show();

    return app.exec();
}
