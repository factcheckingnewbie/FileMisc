#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QLabel>
#include <QModelIndex>
#include <QEvent>

class FileManager : public QMainWindow
{
    Q_OBJECT

public:
    FileManager(QWidget *parent = nullptr);
    ~FileManager();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onTreeViewClicked(const QModelIndex &index);
    void onListView1Clicked(const QModelIndex &index);
    void onListView2Clicked(const QModelIndex &index);
    void onExecuteCommand();

private:
    void setupUI();
    void updateListView2();
    void executeGetFilenameCommand();
    void updateFocusStyle();

    // UI Components
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *rightLayout;
    QVBoxLayout *commandLayout;
    QSplitter *mainSplitter;
    QSplitter *rightSplitter;

    // Views
    QTreeView *treeView;
    QListView *listView1;
    QListView *listView2;

    // Models
    QFileSystemModel *fileSystemModel;
    QFileSystemModel *fileSystemModel2;  // Independent model for listView2
    QStandardItemModel *customModel;

    // Command interface
    QWidget *commandWidget;
    QLabel *commandLabel;
    QLineEdit *commandLineEdit;
    QPushButton *executeButton;

    // Current state
    QString currentPath;
    QModelIndex selectedFileIndex;
};

#endif // FILEMANAGER_H
