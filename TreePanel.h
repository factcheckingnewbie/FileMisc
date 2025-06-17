#pragma once

#include <QWidget>
#include <QTreeView>
#include <KDirModel>
#include <QUrl>

/*
 * TreePanel - extracted from main.cpp
 * Contains a QTreeView with KDirModel, all signals/slots and settings as in main.cpp as of commit d3a744174bba5d55270d3b4313401428a8a62a28.
 */
class TreePanel : public QWidget
{
    Q_OBJECT
public:
    explicit TreePanel(QWidget *parent = nullptr);

    QTreeView* view() const { return m_treeView; }
    KDirModel* model() const { return m_treeModel; }
    QUrl rootUrl() const { return m_treeRootUrl; }

private:
    QTreeView *m_treeView;
    KDirModel *m_treeModel;
    QUrl m_treeRootUrl;
};
