#include "QtFileManager.h"
#include <QApplication>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QDir>
#include <QFileInfo>
#include <QStandardItem>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QLabel>
#include <QWidget>
#include <QFile>

FileManager::FileManager(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainSplitter(nullptr)
    , m_rightSplitter(nullptr)
    , m_treeView(nullptr)
    , m_listView1(nullptr)
    , m_listView2(nullptr)
    , m_commandBox(nullptr)
    , m_executeButton(nullptr)
    , m_fileSystemModel(nullptr)
    , m_listModel1(nullptr)
    , m_listModel2(nullptr)
    , m_currentDirectory(QDir::currentPath())
    , m_selectedFilename("")
    , m_selectedTargetFilename("")
    , m_modelReady(false)
{
    if (!validatePath(m_currentDirectory)) {
        m_currentDirectory = QDir::homePath();
    }
    
    setupUI();
    setupModels();
}

FileManager::~FileManager()
{
    if (m_fileSystemModel) {
        m_fileSystemModel->setParent(nullptr);
    }
    if (m_listModel1) {
        m_listModel1->setParent(nullptr);
    }
    if (m_listModel2) {
        m_listModel2->setParent(nullptr);
    }
}

void FileManager::setupUI()
{
    m_centralWidget = new QWidget(this);
    if (!m_centralWidget) {
        throw std::bad_alloc();
    }
    setCentralWidget(m_centralWidget);
    
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    if (!m_mainSplitter) {
        throw std::bad_alloc();
    }
    
    m_treeView = new QTreeView(this);
    if (!m_treeView) {
        throw std::bad_alloc();
    }
    m_treeView->setMinimumWidth(200);
    
    m_rightSplitter = new QSplitter(Qt::Vertical, this);
    if (!m_rightSplitter) {
        throw std::bad_alloc();
    }
    
    m_listView1 = new QListView(this);
    if (!m_listView1) {
        throw std::bad_alloc();
    }
    m_listView1->setMinimumHeight(150);
    
    QWidget *bottomWidget = new QWidget(this);
    if (!bottomWidget) {
        throw std::bad_alloc();
    }
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomWidget);
    if (!bottomLayout) {
        throw std::bad_alloc();
    }
    
    m_listView2 = new QListView(this);
    if (!m_listView2) {
        throw std::bad_alloc();
    }
    m_listView2->setMinimumHeight(150);
    
    QWidget *commandWidget = new QWidget(this);
    if (!commandWidget) {
        throw std::bad_alloc();
    }
    QHBoxLayout *commandLayout = new QHBoxLayout(commandWidget);
    if (!commandLayout) {
        throw std::bad_alloc();
    }
    
    QLabel *commandLabel = new QLabel("Command:", this);
    if (!commandLabel) {
        throw std::bad_alloc();
    }
    
    m_commandBox = new QLineEdit(this);
    if (!m_commandBox) {
        throw std::bad_alloc();
    }
    m_commandBox->setPlaceholderText("Enter command (e.g., get filename, rename)");
    
    m_executeButton = new QPushButton("Execute", this);
    if (!m_executeButton) {
        throw std::bad_alloc();
    }
    
    commandLayout->addWidget(commandLabel);
    commandLayout->addWidget(m_commandBox);
    commandLayout->addWidget(m_executeButton);
    
    bottomLayout->addWidget(m_listView2);
    bottomLayout->addWidget(commandWidget);
    
    m_rightSplitter->addWidget(m_listView1);
    m_rightSplitter->addWidget(bottomWidget);
    
    m_mainSplitter->addWidget(m_treeView);
    m_mainSplitter->addWidget(m_rightSplitter);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    if (!mainLayout) {
        throw std::bad_alloc();
    }
    mainLayout->addWidget(m_mainSplitter);
    
    m_mainSplitter->setSizes({200, 600});
    m_rightSplitter->setSizes({200, 250});
    
    setWindowTitle("Qt File Manager");
    resize(800, 600);
}

void FileManager::setupModels()
{
    m_fileSystemModel = new QFileSystemModel(this);
    if (!m_fileSystemModel) {
        throw std::bad_alloc();
    }
    
    m_listModel1 = new QStandardItemModel(this);
    if (!m_listModel1) {
        throw std::bad_alloc();
    }
    
    m_listModel2 = new QStandardItemModel(this);
    if (!m_listModel2) {
        throw std::bad_alloc();
    }
    
    connect(m_fileSystemModel, &QFileSystemModel::directoryLoaded,
            this, &FileManager::onDirectoryLoaded);
    
    m_fileSystemModel->setRootPath(m_currentDirectory);
    
    m_treeView->setModel(m_fileSystemModel);
    m_listView1->setModel(m_listModel1);
    m_listView2->setModel(m_listModel2);
}

void FileManager::onDirectoryLoaded(const QString &path)
{
    if (path == m_currentDirectory) {
        m_modelReady = true;
        
        m_treeView->setRootIndex(m_fileSystemModel->index(m_currentDirectory));
        
        for (int i = 1; i < m_fileSystemModel->columnCount(); ++i) {
            m_treeView->hideColumn(i);
        }
        
        connectSignals();
        updateListViews(m_currentDirectory);
    }
}

void FileManager::connectSignals()
{
    if (!m_modelReady) {
        return;
    }
    
    if (m_treeView && m_treeView->selectionModel()) {
        connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
                this, &FileManager::onTreeViewSelectionChanged);
    }
    
    if (m_listView1 && m_listView1->selectionModel()) {
        connect(m_listView1->selectionModel(), &QItemSelectionModel::currentChanged,
                this, &FileManager::onListView1SelectionChanged);
    }
    
    if (m_listView2 && m_listView2->selectionModel()) {
        connect(m_listView2->selectionModel(), &QItemSelectionModel::currentChanged,
                this, &FileManager::onListView2SelectionChanged);
    }
    
    if (m_executeButton) {
        connect(m_executeButton, &QPushButton::clicked,
                this, &FileManager::onCommandExecuted);
    }
    
    if (m_commandBox) {
        connect(m_commandBox, &QLineEdit::returnPressed,
                this, &FileManager::onCommandExecuted);
    }
}

void FileManager::onTreeViewSelectionChanged(const QModelIndex &index)
{
    if (!validateModelIndex(index, m_fileSystemModel) || !m_modelReady) {
        return;
    }
    
    QString selectedPath = m_fileSystemModel->filePath(index);
    if (!validatePath(selectedPath)) {
        return;
    }
    
    QFileInfo fileInfo(selectedPath);
    
    if (fileInfo.exists() && fileInfo.isDir() && fileInfo.isReadable()) {
        m_currentDirectory = selectedPath;
        updateListViews(m_currentDirectory);
    }
}

void FileManager::onListView1SelectionChanged(const QModelIndex &index)
{
    if (!validateModelIndex(index, m_listModel1)) {
        return;
    }
    
    QStandardItem *item = m_listModel1->itemFromIndex(index);
    if (item && !item->text().isEmpty()) {
        m_selectedFilename = item->text();
    }
}

void FileManager::onListView2SelectionChanged(const QModelIndex &index)
{
    if (!validateModelIndex(index, m_listModel2)) {
        return;
    }
    
    QStandardItem *item = m_listModel2->itemFromIndex(index);
    if (item && !item->text().isEmpty()) {
        m_selectedTargetFilename = item->text();
    }
}

void FileManager::onCommandExecuted()
{
    if (!m_commandBox) {
        return;
    }
    
    QString command = m_commandBox->text().trimmed();
    
    if (command.isEmpty()) {
        return;
    }
    
    if (command.toLower() == "get filename" || command.toLower() == "getfilename") {
        if (!m_selectedFilename.isEmpty()) {
            updateListView2WithFilename(m_selectedFilename);
        }
    } else if (command.toLower() == "rename") {
        executeRenameCommand();
    } else {
        QMessageBox::information(this, "Command Info", 
                                QString("Unknown command: %1\n\nAvailable commands:\n- get filename\n- rename").arg(command));
    }
}

void FileManager::updateListViews(const QString &directoryPath)
{
    if (!m_listModel1 || !m_listModel2 || !validatePath(directoryPath)) {
        return;
    }
    
    m_listModel1->clear();
    m_listModel2->clear();
    
    QDir dir(directoryPath);
    if (!dir.exists() || !dir.isReadable()) {
        return;
    }
    
    QFileInfoList fileList;
    try {
        fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    } catch (...) {
        return;
    }
    
    for (const QFileInfo &fileInfo : fileList) {
        if (!fileInfo.isReadable()) {
            continue;
        }
        
        QStandardItem *item1 = new QStandardItem(fileInfo.fileName());
        if (item1) {
            m_listModel1->appendRow(item1);
        }
        
        QStandardItem *item2 = new QStandardItem(fileInfo.fileName());
        if (item2) {
            m_listModel2->appendRow(item2);
        }
    }
}

void FileManager::updateListView2WithFilename(const QString &filename)
{
    if (!m_listModel2 || filename.isEmpty()) {
        return;
    }
    
    m_listModel2->clear();
    
    QStandardItem *item = new QStandardItem(filename);
    if (item) {
        m_listModel2->appendRow(item);
    }
}

void FileManager::executeRenameCommand()
{
    // Check if a file is selected in listView1
    if (m_selectedFilename.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a file from the left list view first.");
        return;
    }
    
    // Check if a target filename is selected in listView2
    if (m_selectedTargetFilename.isEmpty()) {
        QMessageBox::warning(this, "No Target Selection", "Please select a target filename from the right list view.");
        return;
    }
    
    // Check if source and target filenames are the same
    if (m_selectedFilename == m_selectedTargetFilename) {
        QMessageBox::information(this, "No Change", "Source and target filenames are the same. No action needed.");
        return;
    }
    
    // Build the full file paths
    QString sourceFilePath = m_currentDirectory + "/" + m_selectedFilename;
    QString targetFilePath = m_currentDirectory + "/" + m_selectedTargetFilename;
    
    // Check if source file exists
    if (!QFile::exists(sourceFilePath)) {
        QMessageBox::warning(this, "File Not Found", 
                            QString("Source file '%1' not found.").arg(m_selectedFilename));
        return;
    }
    
    // Check if target file already exists
    if (QFile::exists(targetFilePath)) {
        QMessageBox::warning(this, "File Exists", 
                            QString("A file with the name '%1' already exists in this directory.").arg(m_selectedTargetFilename));
        return;
    }
    
    // Perform the rename operation
    QFile sourceFile(sourceFilePath);
    if (sourceFile.rename(targetFilePath)) {
        // Rename successful
        QMessageBox::information(this, "Rename Successful", 
                                QString("File renamed from '%1' to '%2'").arg(m_selectedFilename, m_selectedTargetFilename));
        
        // Update both list views to reflect the change
        updateListViews(m_currentDirectory);
        
        // Clear selections
        m_selectedFilename.clear();
        m_selectedTargetFilename.clear();
        
    } else {
        // Rename failed
        QMessageBox::critical(this, "Rename Failed", 
                             QString("Failed to rename file from '%1' to '%2'. Please check file permissions and try again.").arg(m_selectedFilename, m_selectedTargetFilename));
    }
}

bool FileManager::validateModelIndex(const QModelIndex &index, const QAbstractItemModel *expectedModel) const
{
    return index.isValid() && index.model() == expectedModel;
}

bool FileManager::validatePath(const QString &path) const
{
    if (path.isEmpty()) {
        return false;
    }
    
    QFileInfo fileInfo(path);
    return fileInfo.exists() && fileInfo.isReadable();
}
