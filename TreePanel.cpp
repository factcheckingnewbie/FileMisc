#include "TreePanel.h"
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <KDirLister>
#include <KFileItem>

TreePanel::TreePanel(QWidget *parent)
    : QWidget(parent),
      m_treeView(new QTreeView(this)),
      m_dirModel(new KDirModel(m_treeView)),
      m_rootUrl(QUrl::fromLocalFile(QDir::rootPath()))
{
    m_dirModel->dirLister()->setMimeFilter(QStringList() << "inode/directory");

    m_dirModel->openUrl(m_rootUrl, KDirModel::ShowRoot);
    m_treeView->setModel(m_dirModel);

    QModelIndex rootIndex = m_dirModel->indexForUrl(m_rootUrl);
    m_treeView->setRootIndex(rootIndex);
    m_treeView->setHeaderHidden(true);
    m_treeView->resizeColumnToContents(0);

    qDebug() << "[TreePanel] After setRootIndex:";
    qDebug() << "  rootIndex.isValid() =" << rootIndex.isValid();
    qDebug() << "  rootIndex data =" << rootIndex.data().toString();
    qDebug() << "  rootIndex internalId =" << rootIndex.internalId();

    QTimer::singleShot(0, m_treeView, [this, rootIndex]() {
        qDebug() << "[TreePanel] QTimer::singleShot fired. Expanding root.";
        if (rootIndex.isValid()) {
            m_treeView->expand(rootIndex);
            qDebug() << "[TreePanel] Called expand(rootIndex)";
        } else {
            qDebug() << "[TreePanel] rootIndex is invalid in QTimer";
        }
    });

    connect(m_treeView, &QTreeView::collapsed, this, [this, rootIndex](const QModelIndex &idx) {
        if (idx == rootIndex) {
            m_treeView->expand(rootIndex);
            qDebug() << "[TreePanel] Re-expanded root after collapse";
        }
    });

    connect(m_treeView, &QTreeView::expanded, this, [this](const QModelIndex &) {
        m_treeView->resizeColumnToContents(0);
    });
    connect(m_treeView, &QTreeView::collapsed, this, [this](const QModelIndex &) {
        m_treeView->resizeColumnToContents(0);
    });

    connect(m_treeView, &QTreeView::activated, this, [this](const QModelIndex &index) {
        if (!index.isValid())
            return;
        KFileItem item = m_dirModel->itemForIndex(index);
        if (item.isDir()) {
            emit directoryActivated(item.url());
        }
    });

    setupDebugSignals();
}

QUrl TreePanel::rootUrl() const
{
    return m_rootUrl;
}

KDirModel *TreePanel::model() const
{
    return m_dirModel;
}

QTreeView *TreePanel::view() const
{
    return m_treeView;
}

void TreePanel::setupDebugSignals()
{
    connect(m_dirModel, &QAbstractItemModel::rowsInserted, this,
        [this](const QModelIndex &parent, int first, int last) {
            qDebug() << "[TreePanel] rowsInserted:"
                     << "parent.isValid() =" << parent.isValid()
                     << "first =" << first << "last =" << last;
        });
    connect(m_dirModel, &QAbstractItemModel::layoutChanged, this, []() {
        qDebug() << "[TreePanel] layoutChanged signal fired.";
    });
    connect(m_dirModel, &QAbstractItemModel::modelReset, this, []() {
        qDebug() << "[TreePanel] modelReset signal fired.";
    });
}
