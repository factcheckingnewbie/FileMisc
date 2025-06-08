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
#include <stdexcept>

QtFileManager::QtFileManager(QWidget *parent)
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
    , m_modelReady(false)
{
    if (!validatePath(m_currentDirectory)) {
        m_currentDirectory = QDir::homePath();
    }
    
    try {
        setupUI();
        setupModels();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Initialization Error", 
                             QString("Failed to initialize application: %1").arg(e.what()));
        close();
    }
}

QtFileManager::~QtFileManager()
{
    // Qt's parent-child system will handle cleanup automatically
    // No need to manually set parents to nullptr
}

void QtFileManager::setupUI()
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
    m_commandBox->setPlaceholderText("Enter command (e.g., get filename)");
    
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

void QtFileManager::setupModels()
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
            this, &QtFileManager::onDirectoryLoaded);
    
    m_fileSystemModel->setRootPath(m_currentDirectory);
    
    m_treeView->setModel(m_fileSystemModel);
    m_listView1->setModel(m_listModel1);
    m_listView2->setModel(m_listModel2);
}

void QtFileManager::onDirectoryLoaded(const QString &path)
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

void QtFileManager::connectSignals()
{
    if (!m_modelReady) {
        return;
    }
    
    if (m_treeView && m_treeView->selectionModel()) {
        connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
                this, &QtFileManager::onTreeViewSelectionChanged);
    }
    
    if (m_listView1 && m_listView1->selectionModel()) {
        connect(m_listView1->selectionModel(), &QItemSelectionModel::currentChanged,
                this, &QtFileManager::onListView1SelectionChanged);
    }
    
    if (m_executeButton) {
        connect(m_executeButton, &QPushButton::clicked,
                this, &QtFileManager::onCommandExecuted);
    }
    
    if (m_commandBox) {
        connect(m_commandBox, &QLineEdit::returnPressed,
                this, &QtFileManager::onCommandExecuted);
    }
}

void QtFileManager::onTreeViewSelectionChanged(const QModelIndex &index)
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

void QtFileManager::onListView1SelectionChanged(const QModelIndex &index)
{
    if (!validateModelIndex(index, m_listModel1)) {
        return;
    }
    
    QStandardItem *item = m_listModel1->itemFromIndex(index);
    if (item && !item->text().isEmpty()) {
        m_selectedFilename = item->text();
    }
}

void QtFileManager::onCommandExecuted()
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
        } else {
            QMessageBox::information(this, "No Selection", 
                                   "Please select a file from the list first.");
        }
    } else {
        QMessageBox::information(this, "Unknown Command", 
                               QString("Unknown command: %1\n\nAvailable commands:\n- get filename").arg(command));
    }
}

void QtFileManager::updateListViews(const QString &directoryPath)
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
    } catch (const std::exception&) {
        QMessageBox::warning(this, "Directory Error", 
                           "Failed to read directory contents.");
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

void QtFileManager::updateListView2WithFilename(const QString &filename)
{
    if (!m_listModel2 || filename.isEmpty()) {
        return;
    }
    
    m_listModel2->clear();
    
    QStandardItem *item = new QStandardItem(filename);
    if (item) {
        m_listModel2->appendRow(item);
        QMessageBox::information(this, "Command Executed", 
                               QString("Selected filename displayed: %1").arg(filename));
    }
}

bool QtFileManager::validateModelIndex(const QModelIndex &index, const QAbstractItemModel *expectedModel) const
{
    return index.isValid() && index.model() == expectedModel;
}

bool QtFileManager::validatePath(const QString &path) const
{
    if (path.isEmpty()) {
        return false;
    }
    
    QFileInfo fileInfo(path);
    return fileInfo.exists() && fileInfo.isReadable();
}
