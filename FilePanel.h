#pragma once

#include <QWidget>
#include <QListView>
#include <QFileSystemModel>
#include <KIO/Job>
#include <KIO/ListJob>
#include <KIO/DeleteJob>
#include <KIO/CopyJob>
#include <QUrl>
#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

class FilePanel : public QWidget
{
    Q_OBJECT

public:
    explicit FilePanel(QWidget *parent = nullptr);

    void setDirectory(const QUrl &url);
    QUrl currentDirectory() const;
    QUrl selectedUrl() const;

signals:
    void directoryChanged(const QUrl &url);

public slots:
    void refresh();

private slots:
    void onActivated(const QModelIndex &index);
    void onCustomContextMenu(const QPoint &pos);
    void onDeleteFile();
    void onCopyFile();
    void onRenameFile();

private:
    QFileSystemModel *m_model;
    QListView *m_view;
    QUrl m_currentDir;

    void openSelected();
    void deleteSelected();
    void copySelected();
    void renameSelected();
    void showError(const QString &msg);

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
