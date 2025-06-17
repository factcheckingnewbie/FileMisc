#pragma once

#include <QWidget>
#include <QTreeView>
#include <KDirModel>
#include <QUrl>

/*
 * TreePanel - extracted from main.cpp
 * Contains a QTreeView with KDirModel, all signals/slots and settings as in main.cpp as of commit d3a744174bba5d55270d3b4313401428a8a62a28.
 * 
 * IMPORTANT: view() and model() may return nullptr if construction failed.
 * Always check return values before use.
 */
class TreePanel : public QWidget
{
    Q_OBJECT
public:
    explicit TreePanel(QWidget *parent = nullptr);
    
    // Disable copy and move to prevent slicing
    TreePanel(const TreePanel&) = delete;
    TreePanel& operator=(const TreePanel&) = delete;
    TreePanel(TreePanel&&) = delete;
    TreePanel& operator=(TreePanel&&) = delete;

    // May return nullptr if construction failed
    QTreeView* view() const noexcept { return m_treeView; }
    
    // May return nullptr if construction failed
    KDirModel* model() const noexcept { return m_treeModel; }
    
    QUrl rootUrl() const noexcept { return m_treeRootUrl; }

signals:
    void directorySelected(const QUrl &url);

private:
    QTreeView *m_treeView;
    KDirModel *m_treeModel;
    QUrl m_treeRootUrl;
};
