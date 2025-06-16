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
    qDebug() << "=== Application started ===";

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
     qDebug() << "[DEBUG] After setRootIndex:";
     qDebug() << "  rootIndex.isValid() =" << rootIndex.isValid();
     qDebug() << "  rootIndex data =" << rootIndex.data().toString();
     qDebug() << "  rootIndex internalId =" << rootIndex.internalId();
 
    qDebug() << "[DEBUG] Reached before app.exec()";

     // Prevent user from collapsing the root node
     QObject::connect(treeView, &QTreeView::collapsed, treeView,
         [treeView, rootIndex](const QModelIndex &idx) {
             if (idx == rootIndex) {
                 treeView->expand(rootIndex);
             }
         }
     );
    // Model signals for debug
    QObject::connect(treeModel, &QAbstractItemModel::layoutChanged, treeView, []() {
        qDebug() << "[DEBUG] layoutChanged signal fired.";
    });
    QObject::connect(treeModel, &QAbstractItemModel::modelReset, treeView, []() {
        qDebug() << "[DEBUG] modelReset signal fired.";
    });

    // Try expanding root after layoutChanged/modelReset
    QObject::connect(treeModel, &QAbstractItemModel::layoutChanged, treeView, [treeModel, treeView, treeRootUrl]() {
        const QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
        qDebug() << "[DEBUG] layoutChanged handler: trying expand";
        qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
        if (currentRoot.isValid()) {
            treeView->expand(currentRoot);
            qDebug() << "[DEBUG] Expanded currentRoot in layoutChanged";
        }
    });
    QObject::connect(treeModel, &QAbstractItemModel::modelReset, treeView, [treeModel, treeView, treeRootUrl]() {
        const QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
        qDebug() << "[DEBUG] modelReset handler: trying expand";
        qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
        if (currentRoot.isValid()) {
            treeView->expand(currentRoot);
            qDebug() << "[DEBUG] Expanded currentRoot in modelReset";
        }
    });
     // Expand root node "/" as soon as event loop starts (model will be populated)
     QTimer::singleShot(0, treeView, [treeModel, treeView, treeRootUrl]() {
         const QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
         qDebug() << "[DEBUG] QTimer::singleShot fired. Attempting to expand root.";
         qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
         qDebug() << "  currentRoot data =" << currentRoot.data().toString();
         qDebug() << "  currentRoot internalId =" << currentRoot.internalId();
         if (currentRoot.isValid()) {
             treeView->expand(currentRoot);
             qDebug() << "[DEBUG] Called expand(currentRoot) in QTimer";
         } else {
             qDebug() << "[DEBUG] currentRoot is invalid in QTimer";
         }
     });
 
     // Signal: rowsInserted
     QObject::connect(treeModel, &QAbstractItemModel::rowsInserted, treeView,
         [treeView, treeModel, treeRootUrl](const QModelIndex &parent, int first, int last) {
             const QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
             qDebug() << "[DEBUG] rowsInserted signal fired.";
             qDebug() << "  parent.isValid() =" << parent.isValid();
             qDebug() << "  parent data =" << parent.data().toString();
             qDebug() << "  parent internalId =" << parent.internalId();
             qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
             qDebug() << "  currentRoot data =" << currentRoot.data().toString();
             qDebug() << "  currentRoot internalId =" << currentRoot.internalId();
             if (parent == currentRoot) {
                 treeView->expand(currentRoot);
                 qDebug() << "[DEBUG] Expanded currentRoot in rowsInserted";
             }
         }
     );
 
     // Prevent user from collapsing the root node, always fetch current rootIndex
     QObject::connect(treeView, &QTreeView::collapsed, treeView,
         [treeView, treeModel, treeRootUrl](const QModelIndex &idx) {
             const QModelIndex currentRoot = treeModel->indexForUrl(treeRootUrl);
             qDebug() << "[DEBUG] collapsed signal fired.";
             qDebug() << "  idx.isValid() =" << idx.isValid();
             qDebug() << "  idx data =" << idx.data().toString();
             qDebug() << "  idx internalId =" << idx.internalId();
             qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
             qDebug() << "  currentRoot data =" << currentRoot.data().toString();
             qDebug() << "  currentRoot internalId =" << currentRoot.internalId();
             if (idx == currentRoot) {
                 treeView->expand(currentRoot);
                 qDebug() << "[DEBUG] Re-expanded currentRoot after collapse";
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
     // Prevent user from collapsing the root node
     QObject::connect(treeView, &QTreeView::collapsed, treeView,
         [treeView, rootIndex](const QModelIndex &idx) {
             if (idx == rootIndex) {
                 treeView->expand(rootIndex);
             }
         }
     );

    
    // Prevent user from collapsing the root node
    QObject::connect(treeView, &QTreeView::collapsed, treeView,
        [treeView, rootIndex](const QModelIndex &idx) {
            if (idx == rootIndex) {
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
