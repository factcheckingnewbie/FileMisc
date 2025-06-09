#ifndef QTFILEMANAGER_H
#define QTFILEMANAGER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QTreeView;
class QListView;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QSplitter;
class QFileSystemModel;
class QStandardItemModel;
class QAbstractItemModel;
class QModelIndex;
class QWidget;
class QLabel;
QT_END_NAMESPACE

class FileManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileManager(QWidget *parent = nullptr);
    ~FileManager();

private slots:
    void onTreeViewSelectionChanged(const QModelIndex &index);
    void onListView1SelectionChanged(const QModelIndex &index);
    void onListView2SelectionChanged(const QModelIndex &index);
    void onCommandExecuted();
    void onDirectoryLoaded(const QString &path);

private:
    void setupUI();
    void setupModels();
    void connectSignals();
    void updateListViews(const QString &directoryPath);
    void updateListView2WithFilename(const QString &filename);
    void executeRenameCommand();
    bool validateModelIndex(const QModelIndex &index, const QAbstractItemModel *expectedModel) const;
    bool validatePath(const QString &path) const;
    
    QWidget *m_centralWidget;
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;
    
    QTreeView *m_treeView;
    QListView *m_listView1;
    QListView *m_listView2;
    QLineEdit *m_commandBox;
    QPushButton *m_executeButton;
    
    QFileSystemModel *m_fileSystemModel;
    QStandardItemModel *m_listModel1;
    QStandardItemModel *m_listModel2;
    
    QString m_currentDirectory;
    QString m_selectedFilename;
    QString m_selectedTargetFilename;
    bool m_modelReady;
};

#endif // QTFILEMANAGER_H
