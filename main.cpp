#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QTreeView>
#include <KDirModel>
#include <KDirLister>
#include "FilePanel.h"
#include "DirOnlyProxyModel.h"

// The TreeView is added as the leftmost widget in the splitter.
// It uses its own KDirModel, always rooted at "/" (or QDir::rootPath()).
// FilePanels can be at any subdirectory, but the TreeView always shows the full filesystem from root.
// No synchronization logic yet, but code is structured to allow future improvements.
// Future: Allow hiding/moving the TreeView via UI or settings.

// TODO: In future, make the tree root configurable by user.

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    // TreeView setup (single fixed root at "/"), now with directory-only proxy
    QTreeView *treeView = new QTreeView;
    KDirModel *treeModel = new KDirModel(treeView);
    KDirLister *treeLister = treeModel->dirLister();

    // PROXY MODEL FOR DIRECTORY-ONLY FILTER
    DirOnlyProxyModel *dirOnlyProxy = new DirOnlyProxyModel(treeView);
    dirOnlyProxy->setSourceModel(treeModel);

    QUrl treeRootUrl = QUrl::fromLocalFile(QDir::rootPath()); // Always root ("/")
    treeLister->openUrl(treeRootUrl);

    // Use proxy model for directory-only filtering
    treeView->setModel(dirOnlyProxy);
    // map root index via proxy
    QModelIndex treeRootIndex = treeModel->indexForUrl(treeRootUrl);
    QModelIndex proxyRootIndex = dirOnlyProxy->mapFromSource(treeRootIndex);
    treeView->setRootIndex(proxyRootIndex);

    treeView->setHeaderHidden(true); // Optional minimalism
    
    treeModel->openUrl(treeRootUrl, KDirModel::ShowRoot);
    // FilePanels (unchanged)
    FilePanel *leftPanel = new FilePanel;
    FilePanel *rightPanel = new FilePanel;

    // Start in home and root for demonstration; adapt as you wish.
    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    // MISSING: splitter definition! It must be created before use.
    QSplitter *splitter = new QSplitter(&mainWin);

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
