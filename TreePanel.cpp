#include "TreePanel.h"
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <KDirLister>
#include <QVBoxLayout>
#include <KFileItemDelegate>

TreePanel::TreePanel(QWidget *parent)
    : QWidget(parent)
    , m_treeView(nullptr)
    , m_treeModel(nullptr)
    , m_treeRootUrl(QUrl::fromLocalFile(QDir::rootPath()))
{
    try {
        m_treeView = new QTreeView(this);
        if (!m_treeView) {
            qDebug() << "[ERROR] Failed to create QTreeView in TreePanel";
            throw std::runtime_error("Failed to create QTreeView");
        }
        
        m_treeModel = new KDirModel(m_treeView);
        if (!m_treeModel) {
            qDebug() << "[ERROR] Failed to create KDirModel in TreePanel";
            throw std::runtime_error("Failed to create KDirModel");
        }
    } catch (const std::exception& e) {
        qDebug() << "[ERROR] Exception in TreePanel constructor:" << e.what();
        // Clean up if partially constructed
        delete m_treeModel;
        m_treeModel = nullptr;
        delete m_treeView;
        m_treeView = nullptr;
        throw; // Re-throw to notify caller
    } catch (...) {
        qDebug() << "[ERROR] Unknown exception in TreePanel constructor";
        delete m_treeModel;
        m_treeModel = nullptr;
        delete m_treeView;
        m_treeView = nullptr;
        throw;
    }

    // Only show directories in the TreeView using public KIO API
    KDirLister* lister = m_treeModel->dirLister();
    if (lister) {
        lister->setMimeFilter(QStringList() << "inode/directory");
    } else {
        qDebug() << "[WARNING] dirLister() returned null";
    }

    m_treeModel->openUrl(m_treeRootUrl, KDirModel::ShowRoot); // Show "/" as top node
    m_treeView->setModel(m_treeModel);
    
    KFileItemDelegate* delegate = new KFileItemDelegate(m_treeView);
    if (delegate) {
        m_treeView->setItemDelegate(delegate);
    } else {
        qDebug() << "[WARNING] Failed to create KFileItemDelegate";
    }
    
    QModelIndex rootIndex = m_treeModel->indexForUrl(m_treeRootUrl);
    m_treeView->setRootIndex(rootIndex);
    m_treeView->setHeaderHidden(true); // Optional minimalism
    m_treeView->resizeColumnToContents(0);

    qDebug() << "[DEBUG] After setRootIndex:";
    qDebug() << "  rootIndex.isValid() =" << rootIndex.isValid();
    qDebug() << "  rootIndex data =" << rootIndex.data().toString();
    qDebug() << "  rootIndex internalId =" << rootIndex.internalId();

    qDebug() << "[DEBUG] Reached before event loop in TreePanel";

    // Explicit QTimer test for unconditional debug
    QTimer::singleShot(0, m_treeView, []() {
        qDebug() << "[DEBUG] QTimer fired (unconditional test)";
    });

    // Model signals for debug
    QObject::connect(m_treeModel, &QAbstractItemModel::layoutChanged, m_treeView, []() {
        qDebug() << "[DEBUG] layoutChanged signal fired.";
    });
    QObject::connect(m_treeModel, &QAbstractItemModel::modelReset, m_treeView, []() {
        qDebug() << "[DEBUG] modelReset signal fired.";
    });

    // Try expanding root after layoutChanged/modelReset
    QObject::connect(m_treeModel, &QAbstractItemModel::layoutChanged, m_treeView,
        [this]() {
            if (!m_treeModel) {
                qDebug() << "[ERROR] m_treeModel is null in layoutChanged handler";
                return;
            }
            const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
            qDebug() << "[DEBUG] layoutChanged handler: trying expand";
            qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
            if (currentRoot.isValid() && m_treeView) {
                m_treeView->expand(currentRoot);
                qDebug() << "[DEBUG] Expanded currentRoot in layoutChanged";
            }
        });
    QObject::connect(m_treeModel, &QAbstractItemModel::modelReset, m_treeView,
        [this]() {
            if (!m_treeModel) {
                qDebug() << "[ERROR] m_treeModel is null in modelReset handler";
                return;
            }
            const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
            qDebug() << "[DEBUG] modelReset handler: trying expand";
            qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
            if (currentRoot.isValid() && m_treeView) {
                m_treeView->expand(currentRoot);
                qDebug() << "[DEBUG] Expanded currentRoot in modelReset";
            }
        });

    // Expand root node "/" as soon as event loop starts (model will be populated)
    QTimer::singleShot(0, m_treeView, [this]() {
        if (!m_treeModel) {
            qDebug() << "[ERROR] m_treeModel is null in QTimer handler";
            return;
        }
        const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
        qDebug() << "[DEBUG] QTimer::singleShot fired. Attempting to expand root.";
        qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
        qDebug() << "  currentRoot data =" << currentRoot.data().toString();
        qDebug() << "  currentRoot internalId =" << currentRoot.internalId();
        if (currentRoot.isValid() && m_treeView) {
            m_treeView->expand(currentRoot);
            qDebug() << "[DEBUG] Called expand(currentRoot) in QTimer";
        } else {
            qDebug() << "[DEBUG] currentRoot is invalid or m_treeView is null in QTimer";
        }
    });

    // Signal: rowsInserted
    QObject::connect(m_treeModel, &QAbstractItemModel::rowsInserted, m_treeView,
        [this](const QModelIndex &parent, int first, int last) {
            if (!m_treeModel) {
                qDebug() << "[ERROR] m_treeModel is null in rowsInserted handler";
                return;
            }
            const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
            qDebug() << "[DEBUG] rowsInserted signal fired.";
            qDebug() << "  parent.isValid() =" << parent.isValid();
            qDebug() << "  parent data =" << parent.data().toString();
            qDebug() << "  parent internalId =" << parent.internalId();
            qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
            qDebug() << "  currentRoot data =" << currentRoot.data().toString();
            qDebug() << "  currentRoot internalId =" << currentRoot.internalId();
            if (parent == currentRoot && m_treeView) {
                m_treeView->expand(currentRoot);
                qDebug() << "[DEBUG] Expanded currentRoot in rowsInserted";
            }
        }
    );

    // Prevent user from collapsing the root node, always fetch current rootIndex
    QObject::connect(m_treeView, &QTreeView::collapsed, m_treeView,
        [this](const QModelIndex &idx) {
            if (!m_treeModel) {
                qDebug() << "[ERROR] m_treeModel is null in collapsed handler";
                return;
            }
            const QModelIndex currentRoot = m_treeModel->indexForUrl(m_treeRootUrl);
            qDebug() << "[DEBUG] collapsed signal fired.";
            qDebug() << "  idx.isValid() =" << idx.isValid();
            qDebug() << "  idx data =" << idx.data().toString();
            qDebug() << "  idx internalId =" << idx.internalId();
            qDebug() << "  currentRoot.isValid() =" << currentRoot.isValid();
            qDebug() << "  currentRoot data =" << currentRoot.data().toString();
            qDebug() << "  currentRoot internalId =" << currentRoot.internalId();
            if (idx == currentRoot && m_treeView) {
                m_treeView->expand(currentRoot);
                qDebug() << "[DEBUG] Re-expanded currentRoot after collapse";
            }
        }
    );
    // Ensure root expands as soon as children are loaded
    QObject::connect(m_treeModel, &QAbstractItemModel::rowsInserted, m_treeView,
        [this, rootIndex](const QModelIndex &parent, int, int) {
            if (parent == rootIndex && m_treeView) {
                m_treeView->expand(rootIndex);
            }
        }
    );
    // Prevent user from collapsing the root node
    QObject::connect(m_treeView, &QTreeView::collapsed, m_treeView,
        [this, rootIndex](const QModelIndex &idx) {
            if (idx == rootIndex && m_treeView) {
                m_treeView->expand(rootIndex);
            }
        }
    );
    // Prevent user from collapsing the root node
    QObject::connect(m_treeView, &QTreeView::collapsed, m_treeView,
        [this, rootIndex](const QModelIndex &idx) {
            if (idx == rootIndex && m_treeView) {
                m_treeView->expand(rootIndex);
            }
        }
    );
    // Auto-resize on expand/collapse
    QObject::connect(m_treeView, &QTreeView::expanded, m_treeView, [this](const QModelIndex &) {
        if (m_treeView) {
            m_treeView->resizeColumnToContents(0);
        }
    });
    QObject::connect(m_treeView, &QTreeView::collapsed, m_treeView, [this](const QModelIndex &) {
        if (m_treeView) {
            m_treeView->resizeColumnToContents(0);
        }
    });

    // Auto-resize on model changes (directory listing updates)
    QObject::connect(m_treeModel, &QAbstractItemModel::rowsInserted, m_treeView, [this](const QModelIndex &, int, int) {
        if (m_treeView) {
            m_treeView->resizeColumnToContents(0);
        }
    });
    QObject::connect(m_treeModel, &QAbstractItemModel::dataChanged, m_treeView, [this](const QModelIndex &, const QModelIndex &, const QVector<int> &) {
        if (m_treeView) {
            m_treeView->resizeColumnToContents(0);
        }
    });

    // Use layout to ensure widget expands to full panel
    QVBoxLayout *layout = new QVBoxLayout(this);
    if (layout && m_treeView) {
        layout->addWidget(m_treeView);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
    } else {
        qDebug() << "[ERROR] Failed to create layout or m_treeView is null";
    }

    // Directory selection emission
    connect(m_treeView, &QTreeView::clicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        if (!m_treeModel) {
            qDebug() << "[ERROR] m_treeModel is null in clicked handler";
            return;
        }
        KFileItem item = m_treeModel->itemForIndex(index);
        if (item.isDir())
            emit directorySelected(item.url());
    });
}
