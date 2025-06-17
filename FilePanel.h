#pragma once

#include <QWidget>
#include <QListView>
#include <KDirModel>
#include <QUrl>

/*
 * FilePanel - extracted from main.cpp
 * Contains a QListView with KDirModel, all signals/slots and settings as in main.cpp as of commit d3a744174bba5d55270d3b4313401428a8a62a28.
 * 
 * IMPORTANT: view() and model() may return nullptr if construction failed.
 * Always check return values before use.
 */
class FilePanel : public QWidget
{
    Q_OBJECT
public:
    explicit FilePanel(QWidget *parent = nullptr);
    
    // Disable copy and move to prevent slicing
    FilePanel(const FilePanel&) = delete;
    FilePanel& operator=(const FilePanel&) = delete;
    FilePanel(FilePanel&&) = delete;
    FilePanel& operator=(FilePanel&&) = delete;

    void setDirectory(const QUrl &url);
    QUrl currentDirectory() const noexcept;

    // May return nullptr if construction failed
    QListView* view() const noexcept { return m_listView; }
    
    // May return nullptr if construction failed
    KDirModel* model() const noexcept { return m_fileModel; }

signals:
    void directoryChanged(const QUrl &newUrl);
    void fileSelected(const QUrl &fileUrl);
    void fileDoubleClicked(const QUrl &fileUrl);

private:
    QListView *m_listView;
    KDirModel *m_fileModel;
    QUrl m_currentUrl;
};
