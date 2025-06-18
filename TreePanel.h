#pragma once

#include <QWidget>
#include <QTreeView>
#include <QUrl>
#include <KDirModel>

class TreePanel : public QWidget
{
    Q_OBJECT
public:
    explicit TreePanel(QWidget *parent = nullptr);
    
    // Add method to navigate to a specific path
    void navigateToPath(const QUrl &url);

signals:
    void directorySelected(const QUrl &url);

private:
    QTreeView *m_treeView;
    KDirModel *m_treeModel;
    QUrl m_treeRootUrl;
};
