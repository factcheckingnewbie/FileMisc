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
    treeView->setHeaderHidden(true); // Optional minimalism
    treeView->resizeColumnToContents(0);

    // Expand all ancestors down to $HOME after model is populated
    QString homePath = QDir::homePath();
    QUrl homeUrl = QUrl::fromLocalFile(homePath);

    // Helper: expand recursively from root to $HOME
    auto expandToHome = [=]() {
        QModelIndex homeIndex = treeModel->indexForUrl(homeUrl);
        if (!homeIndex.isValid()) {
            qDebug() << "[DEBUG] expandToHome: homeIndex is not valid, skipping";
            return;
        }
        // Expand all ancestors
        QList<QModelIndex> toExpand;
        QModelIndex idx = homeIndex;
        while (idx.isValid()) {
            toExpand.prepend(idx);
            idx = idx.parent();
        }
        for (const QModelIndex &i : toExpand) {
            treeView->expand(i);
        }
        treeView->scrollTo(homeIndex);
        treeView->setCurrentIndex(homeIndex);
    };

    // Call expandToHome after event loop starts (model is likely populated)
    QTimer::singleShot(0, treeView, expandToHome);

    // Prevent user from collapsing the root node (always up-to-date)
    QObject::connect(treeView, &QTreeView::collapsed, treeView,
        [treeView, treeModel, treeRootUrl](const QModelIndex &idx) {
            QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
            if (idx == currentRoot) {
                treeView->expand(currentRoot);
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
