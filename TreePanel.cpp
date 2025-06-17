#pragma once

#include <QWidget>
#include <QTreeView>
#include <KDirModel>
#include <QUrl>

class TreePanel : public QWidget
{
    Q_OBJECT

public:
    explicit TreePanel(QWidget *parent = nullptr);

    QUrl rootUrl() const;
    KDirModel *model() const;
    QTreeView *view() const;

signals:
    void directoryActivated(const QUrl &url);

private:
    QTreeView *m_treeView;
    KDirModel *m_dirModel;
    QUrl m_rootUrl;

    void setupDebugSignals();
};
