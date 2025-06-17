#include "TreePanel.h"
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <KDirLister>
#include <QVBoxLayout>

TreePanel::TreePanel(QWidget *parent)
    : QWidget(parent)
    , m_treeView(new QTreeView(this))
    , m_treeModel(new KDirModel(m_treeView))
    , m_treeRootUrl(QUrl::fromLocalFile(QDir::rootPath()))
{
    m_treeModel->dirLister()->setMimeFilter(QStringList() << "inode/directory");
    m_treeModel->openUrl(m_treeRootUrl, KDirModel::ShowRoot);
    m_treeView->setModel(m_treeModel);
    QModelIndex rootIndex = m_treeModel->indexForUrl(m_treeRootUrl);
    m_treeView->setRootIndex(rootIndex);
    m_treeView->setHeaderHidden(true);
    m_treeView->resizeColumnToContents(0);

    qDebug() << "[DEBUG] After setRootIndex:";
    qDebug() << "  rootIndex.isValid() =" << rootIndex.isValid();
    qDebug() << "  rootIndex data =" << rootIndex.data().toString();
    qDebug() << "  rootIndex internalId =" << rootIndex.internalId();

    // Diagnostics
    QTimer::singleShot(0, m_treeView, []() {
        qDebug() << "[DEBUG] QTimer fired (unconditional test)";
    });

    // Always expand root node after changes
    auto expandRoot = [this, rootIndex]() {
        if (rootIndex.isValid()) {
            m_treeView->expand(rootIndex);
            qDebug() << "[DEBUG] Expanded currentRoot";
        }
    };

    QObject::connect(m_treeModel, &QAbstractItemModel::layoutChanged, m_treeView, expandRoot);
    QObject::connect(m_treeModel, &QAbstractItemModel::modelReset, m_treeView, expandRoot);
    QObject::connect(m_treeModel, &QAbstractItemModel::rowsInserted, m_treeView, [this, rootIndex](const QModelIndex &parent, int, int) {
        if (parent == rootIndex) m_treeView->expand(rootIndex);
    });
    QObject::connect(m_treeView, &QTreeView::collapsed, m_treeView, [this, rootIndex](const QModelIndex &idx) {
        if (idx == rootIndex) m_treeView->expand(rootIndex);
    });

    // Auto-resize column
    QObject::connect(m_treeView, &QTreeView::expanded, m_treeView, [this](const QModelIndex &) {
        m_treeView->resizeColumnToContents(0);
    });
    QObject::connect(m_treeView, &QTreeView::collapsed, m_treeView, [this](const QModelIndex &) {
        m_treeView->resizeColumnToContents(0);
    });
    QObject::connect(m_treeModel, &QAbstractItemModel::rowsInserted, m_treeView, [this](const QModelIndex &, int, int) {
        m_treeView->resizeColumnToContents(0);
    });
    QObject::connect(m_treeModel, &QAbstractItemModel::dataChanged, m_treeView, [this](const QModelIndex &, const QModelIndex &, const QVector<int> &) {
        m_treeView->resizeColumnToContents(0);
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_treeView);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}
