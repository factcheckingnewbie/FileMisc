#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QTreeView>
#include <KDirModel>
#include <KDirLister>
#include <QPushButton>
#include <QVBoxLayout>
#include "FilePanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    printf("Debug output test: main() started\n");

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    // Create button and layout for top row
    QPushButton *expandHomeButton = new QPushButton("Expand to $HOME");
    QHBoxLayout *topRowLayout = new QHBoxLayout;
    topRowLayout->addWidget(expandHomeButton);
    topRowLayout->addStretch();

    QSplitter *splitter = new QSplitter(&mainWin);

    // TreeView setup (single fixed root at "/")
    QTreeView *treeView = new QTreeView;
    KDirModel *treeModel = new KDirModel(treeView);

    treeModel->dirLister()->setMimeFilter(QStringList() << "inode/directory");

    QUrl treeRootUrl = QUrl::fromLocalFile(QDir::rootPath()); // Always root ("/")
    treeModel->openUrl(treeRootUrl, KDirModel::ShowRoot);     // Show "/" as top node
    treeView->setModel(treeModel);
    QModelIndex rootIndex = treeModel->indexForUrl(treeRootUrl);
    treeView->setRootIndex(rootIndex);
    treeView->setHeaderHidden(false);
    treeView->resizeColumnToContents(0);

    // "Expand to $HOME": expand from / stepwise
    QObject::connect(expandHomeButton, &QPushButton::clicked, treeView, [treeModel, treeView]() {
        QString homePath = QDir::homePath();
        QUrl homeUrl = QUrl::fromLocalFile(homePath);

        // Build path chain from root ("/") to $HOME
        QStringList parts = QDir(homePath).absolutePath().split('/', Qt::SkipEmptyParts);
        QString currentPath = "/";
        QModelIndex parentIdx = treeModel->indexForUrl(QUrl::fromLocalFile(currentPath));
        bool found = parentIdx.isValid();
        if (!found) {
            printf("[DEBUG] Could not get root index\n");
            return;
        }
        treeView->expand(parentIdx);

        // Stepwise expand each ancestor
        for (const QString &part : parts) {
            currentPath += (currentPath.endsWith('/') ? "" : "/") + part;
            QUrl currentUrl = QUrl::fromLocalFile(currentPath);
            QModelIndex idx = treeModel->indexForUrl(currentUrl);
            if (!idx.isValid()) {
                printf("[DEBUG] Index not valid for: %s\n", currentPath.toUtf8().constData());
                return;
            }
            treeView->expand(idx);
            parentIdx = idx;
        }
        // Select and scroll to $HOME
        treeView->scrollTo(parentIdx, QTreeView::EnsureVisible);
        treeView->setCurrentIndex(parentIdx);
        printf("[DEBUG] Expanded to: %s\n", homePath.toUtf8().constData());
    });

    FilePanel *leftPanel = new FilePanel;
    FilePanel *rightPanel = new FilePanel;

    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    splitter->addWidget(treeView);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topRowLayout);
    mainLayout->addWidget(splitter);
    mainWin.setLayout(mainLayout);
    mainWin.resize(1200, 600);
    mainWin.show();

    return app.exec();
}
