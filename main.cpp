#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QTreeView>
#include <QFileSystemModel>
#include "FilePanel.h"

// The TreeView is added as the leftmost widget in the splitter.
// It uses its own QFileSystemModel, independent from the FilePanels.
// No synchronization logic yet, but code is structured to allow future improvements.
// Future: Allow hiding/moving the TreeView via UI or settings.

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    QSplitter *splitter = new QSplitter(&mainWin);

    // TreeView setup (minimal, independent model)
    QTreeView *treeView = new QTreeView;
    QFileSystemModel *treeModel = new QFileSystemModel(treeView);
    treeModel->setRootPath(QDir::rootPath());
    treeView->setModel(treeModel);
    treeView->setRootIndex(treeModel->index(QDir::rootPath()));
    treeView->setHeaderHidden(true); // Optional minimalism

    // FilePanels (unchanged)
    FilePanel *leftPanel = new FilePanel;
    FilePanel *rightPanel = new FilePanel;

    // Start in home and root for demonstration; adapt as you wish.
    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    splitter->addWidget(treeView);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);
    mainWin.setLayout(layout);
    mainWin.resize(1200, 600);
    mainWin.show();

    return app.exec();
}
