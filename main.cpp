#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include "FilePanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    QSplitter *splitter = new QSplitter(&mainWin);
    FilePanel *leftPanel = new FilePanel;
    FilePanel *rightPanel = new FilePanel;

    // Start in home and root for demonstration; adapt as you wish.
    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);
    mainWin.setLayout(layout);
    mainWin.resize(1000, 600);
    mainWin.show();

    return app.exec();
}
