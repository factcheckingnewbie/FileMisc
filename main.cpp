#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QTreeView>
#include <KDirModel>
#include <KDirLister>
#include "FilePanel.h"

// Only KIO public API, per /usr/include/KF6/KIOWidgets/kdirmodel.h

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    QSplitter *splitter = new QSplitter(&mainWin);

    // TreeView setup (single fixed root at "/")
    QTreeView *treeView = new QTreeView;
    KDirModel *treeModel = new KDirModel(treeView);

    // Only show directories in the TreeView using public KIO API
    treeModel->dirLister()->setMimeFilter(QStringList() << "inode/directory");

    QUrl treeRootUrl = QUrl::fromLocalFile(QDir::rootPath()); // Always root ("/")
    treeView->setModel(treeModel);
    treeView->setRootIndex(treeModel->indexForUrl(treeRootUrl));
    treeView->setHeaderHidden(true); // Optional minimalism

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
