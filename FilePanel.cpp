#include "FilePanel.h"
#include <QVBoxLayout>
#include <KFileItemDelegate>
#include <KFileItem>
#include <QDebug>

FilePanel::FilePanel(QWidget *parent)
    : QWidget(parent)
    , m_listView(nullptr)
    , m_fileModel(nullptr)
{
    try {
        m_listView = new QListView(this);
        if (!m_listView) {
            qDebug() << "[ERROR] Failed to create QListView in FilePanel";
            throw std::runtime_error("Failed to create QListView");
        }
        
        m_fileModel = new KDirModel(m_listView);
        if (!m_fileModel) {
            qDebug() << "[ERROR] Failed to create KDirModel in FilePanel";
            throw std::runtime_error("Failed to create KDirModel");
        }
    } catch (const std::exception& e) {
        qDebug() << "[ERROR] Exception in FilePanel constructor:" << e.what();
        // Clean up if partially constructed
        delete m_fileModel;
        m_fileModel = nullptr;
        delete m_listView;
        m_listView = nullptr;
        throw; // Re-throw to notify caller
    } catch (...) {
        qDebug() << "[ERROR] Unknown exception in FilePanel constructor";
        delete m_fileModel;
        m_fileModel = nullptr;
        delete m_listView;
        m_listView = nullptr;
        throw;
    }

    m_listView->setModel(m_fileModel);
    
    KFileItemDelegate* delegate = new KFileItemDelegate(m_listView);
    if (delegate) {
        m_listView->setItemDelegate(delegate);
    } else {
        qDebug() << "[WARNING] Failed to create KFileItemDelegate";
    }
    
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setUniformItemSizes(true);
    m_listView->setResizeMode(QListView::Adjust);

    // Use layout to ensure widget expands to full panel
    QVBoxLayout *layout = new QVBoxLayout(this);
    if (layout && m_listView) {
        layout->addWidget(m_listView);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
    } else {
        qDebug() << "[ERROR] Failed to create layout or m_listView is null";
    }

    // Double-click navigation
    connect(m_listView, &QListView::doubleClicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        if (!m_fileModel) {
            qDebug() << "[ERROR] m_fileModel is null in doubleClicked handler";
            return;
        }
        try {
            KFileItem item = m_fileModel->itemForIndex(index);
            if (!item.isNull()) {
                if (item.isDir()) {
                    setDirectory(item.url());
                } else {
                    emit fileDoubleClicked(item.url());
                }
            } else {
                qDebug() << "[WARNING] itemForIndex returned null KFileItem";
            }
        } catch (const std::exception& e) {
            qDebug() << "[ERROR] Exception in doubleClicked handler:" << e.what();
        } catch (...) {
            qDebug() << "[ERROR] Unknown exception in doubleClicked handler";
        }
    });

    // Single click file selection
    connect(m_listView, &QListView::clicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        if (!m_fileModel) {
            qDebug() << "[ERROR] m_fileModel is null in clicked handler";
            return;
        }
        try {
            KFileItem item = m_fileModel->itemForIndex(index);
            if (!item.isNull()) {
                if (!item.isDir()) {
                    emit fileSelected(item.url());
                }
            } else {
                qDebug() << "[WARNING] itemForIndex returned null KFileItem in clicked handler";
            }
        } catch (const std::exception& e) {
            qDebug() << "[ERROR] Exception in clicked handler:" << e.what();
        } catch (...) {
            qDebug() << "[ERROR] Unknown exception in clicked handler";
        }
    });
}

void FilePanel::setDirectory(const QUrl &url)
{
    if (m_currentUrl != url) {
        m_currentUrl = url;
        if (m_fileModel) {
            try {
                m_fileModel->openUrl(url);
                emit directoryChanged(url);
                qDebug() << "[DEBUG] FilePanel directory changed to:" << url.toString();
            } catch (const std::exception& e) {
                qDebug() << "[ERROR] Exception in setDirectory:" << e.what();
            } catch (...) {
                qDebug() << "[ERROR] Unknown exception in setDirectory";
            }
        } else {
            qDebug() << "[ERROR] m_fileModel is null in setDirectory";
        }
    }
}

QUrl FilePanel::currentDirectory() const
{
    return m_currentUrl;
}
