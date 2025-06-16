#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QTreeView>
#include <KDirModel>
#include <KDirLister>
#include <QTimer>
#include <QDebug>
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
    treeModel->openUrl(treeRootUrl, KDirModel::ShowRoot);     // <-- Show "/" as top node
    treeView->setModel(treeModel);
    QModelIndex rootIndex = treeModel->indexForUrl(treeRootUrl);
    treeView->setRootIndex(rootIndex);
    treeView->setHeaderHidden(true); // Optional minimalism
    treeView->resizeColumnToContents(0);

    // Expand root node "/" as soon as event loop starts (model will be populated)
    QTimer::singleShot(0, treeView, [treeModel, treeView, treeRootUrl]() {
        const QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
        if (currentRoot.isValid()) {
            treeView->expand(currentRoot);
        }
    });

    // Prevent user from collapsing the root node, always fetch current rootIndex
    QObject::connect(treeView, &QTreeView::collapsed, treeView,
        [treeView, treeModel, treeRootUrl](const QModelIndex &idx) {
            const QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
            if (idx == currentRoot) {
                treeView->expand(currentRoot);
            }
        }
    );

    // Ensure root expands as soon as children are loaded
    QObject::connect(treeModel, &QAbstractItemModel::rowsInserted, treeView,
        [treeView, rootIndex](const QModelIndex &parent, int, int) {
            if (parent == rootIndex) {
                treeView->expand(rootIndex);
            }
        }
    );

    // Auto-resize on expand/collapse
    QObject::connect(treeView, &QTreeView::expanded, treeView, [treeView](const QModelIndex &) {
        treeView->resizeColumnToContents(0);
    });
    QObject::connect(treeView, &QTreeView::collapsed, treeView, [treeView](const QModelIndex &) {
        treeView->resizeColumnToContents(0);
    });

    // Auto-resize on model changes (directory listing updates)
    QObject::connect(treeModel, &QAbstractItemModel::rowsInserted, treeView, [treeView](const QModelIndex &, int, int) {
        treeView->resizeColumnToContents(0);
    });
    QObject::connect(treeModel, &QAbstractItemModel::dataChanged, treeView, [treeView](const QModelIndex &, const QModelIndex &, const QVector<int> &) {
        treeView->resizeColumnToContents(0);
    });

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
