#include "TreePanel.h"
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <KDirLister>
#include <QVBoxLayout>
#include <KFileItemDelegate>
#include <QFileInfo>

TreePanel::TreePanel(QWidget *parent)
    : QWidget(parent)
    , m_treeView(new QTreeView(this))
    , m_treeModel(new KDirModel(m_treeView))
    , m_treeRootUrl(QUrl::fromLocalFile(QDir::rootPath()))
{
    // Only show directories in the TreeView using public KIO API
    m_treeModel->dirLister()->setMimeFilter(QStringList() << "inode/directory");

    m_treeModel->openUrl(m_treeRootUrl, KDirModel::ShowRoot); // Show "/" as top node
    m_treeView->setModel(m_treeModel);
    m_treeView->setItemDelegate(new KFileItemDelegate(m_treeView));
    
    // Set root index after model is set
    QModelIndex rootIndex = m_treeModel->indexForUrl(m_treeRootUrl);
    m_treeView->setRootIndex(rootIndex);
    m_treeView->setHeaderHidden(true); // Optional minimalism
    m_treeView->resizeColumnToContents(0);

    qDebug() << "[DEBUG] TreePanel initialized:";
    qDebug() << "  rootIndex.isValid() =" << rootIndex.isValid();
    qDebug() << "  rootIndex data =" << rootIndex.data().toString();

    // PRIMARY METHOD: Connect to KDirLister's completed signal to expand root when fully loaded
    QObject::connect(m_treeModel->dirLister(), 
        static_cast<void(KDirLister::*)()>(&KDirLister::completed), 
        this,
        [this]() {
            const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
            qDebug() << "[DEBUG] KDirLister completed. Checking root expansion.";
            qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
            qDebug() << "  currentRoot.hasChildren() =" << m_treeModel->hasChildren(currentRoot);
            qDebug() << "  currentRoot.isExpanded() =" << m_treeView->isExpanded(currentRoot);
            qDebug() << "  rowCount =" << m_treeModel->rowCount(currentRoot);
            
            // Only expand if valid, has children, and not already expanded
            if (currentRoot.isValid() && 
                m_treeModel->hasChildren(currentRoot) && 
                !m_treeView->isExpanded(currentRoot)) {
                m_treeView->expand(currentRoot);
                qDebug() << "[DEBUG] Expanded root after KDirLister completed";
            }
        });

    // FALLBACK METHOD: QTimer as safety net if KDirLister signal fails
    QTimer::singleShot(200, this, [this]() {
        const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
        qDebug() << "[DEBUG] QTimer fallback check:";
        qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
        qDebug() << "  currentRoot.hasChildren() =" << m_treeModel->hasChildren(currentRoot);
        qDebug() << "  currentRoot.isExpanded() =" << m_treeView->isExpanded(currentRoot);
        
        // Only expand if valid, has children, and not already expanded
        if (currentRoot.isValid() && 
            m_treeModel->hasChildren(currentRoot) && 
            !m_treeView->isExpanded(currentRoot)) {
            m_treeView->expand(currentRoot);
            qDebug() << "[DEBUG] Expanded root via QTimer fallback";
        }
    });

    // Prevent user from collapsing the root node
    QObject::connect(m_treeView, &QTreeView::collapsed, this,
        [this](const QModelIndex &idx) {
            const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
            
            // Check both direct comparison and by URL
            if (idx == currentRoot || m_treeModel->itemForIndex(idx).url() == m_treeRootUrl) {
                qDebug() << "[DEBUG] Root collapse prevented, re-expanding";
                m_treeView->expand(currentRoot);
            }
        }
    );

    // Auto-resize on expand/collapse
    QObject::connect(m_treeView, &QTreeView::expanded, this, [this](const QModelIndex &) {
        m_treeView->resizeColumnToContents(0);
    });
    QObject::connect(m_treeView, &QTreeView::collapsed, this, [this](const QModelIndex &) {
        m_treeView->resizeColumnToContents(0);
    });

    // Auto-resize on model changes (directory listing updates)
    QObject::connect(m_treeModel, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &, int, int) {
        m_treeView->resizeColumnToContents(0);
    });
    QObject::connect(m_treeModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &, const QModelIndex &, const QVector<int> &) {
        m_treeView->resizeColumnToContents(0);
    });

    // Use layout to ensure widget expands to full panel
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_treeView);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // Directory selection emission - THIS IS CORRECT, NOT A BUG
    connect(m_treeView, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        KFileItem item = m_treeModel->itemForIndex(index);
        if (item.isDir())
            emit directorySelected(item.url());
    });
}

void TreePanel::navigateToPath(const QUrl &url)
{
    qDebug() << "[DEBUG] TreePanel::navigateToPath called with:" << url.toString();
    
    // Simple validation - just check if it's a real path
    QString path = url.toLocalFile();
    QFileInfo fileInfo(path);
    
    if (!fileInfo.exists() || !fileInfo.isDir()) {
        qDebug() << "[WARNING] Path does not exist or is not a directory:" << path;
        qDebug() << "[WARNING] Navigation aborted. Please provide a valid directory path.";
        return;
    }
    
    // Simple approach: use KDirModel's expandToUrl and then select
    m_treeModel->expandToUrl(url);
    
    // Wait a bit for model to load, then select and scroll
    QTimer::singleShot(500, this, [this, url]() {
        QModelIndex targetIndex = m_treeModel->indexForUrl(url);
        if (targetIndex.isValid()) {
            m_treeView->setCurrentIndex(targetIndex);
            m_treeView->scrollTo(targetIndex, QAbstractItemView::PositionAtCenter);
            qDebug() << "[DEBUG] Navigation complete to:" << url.toString();
        } else {
            qDebug() << "[WARNING] Could not find index for path:" << url.toString();
        }
    });
}
