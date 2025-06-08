#include "filemanager.h"
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QHeaderView>

FileManager::FileManager(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , rightLayout(nullptr)
    , commandLayout(nullptr)
    , mainSplitter(nullptr)
    , rightSplitter(nullptr)
    , treeView(nullptr)
    , listView1(nullptr)
    , listView2(nullptr)
    , fileSystemModel(nullptr)
    , customModel(nullptr)
    , commandWidget(nullptr)
    , commandLabel(nullptr)
    , commandLineEdit(nullptr)
    , executeButton(nullptr)
{
    setupUI();
    setWindowTitle("Qt File Manager");
    resize(1000, 600);
}

FileManager::~FileManager()
{
}

void FileManager::setupUI()
{
    // Create central widget
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main horizontal splitter
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Setup file system model
    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::rootPath());
    
    // Setup tree view (left panel)
    treeView = new QTreeView(this);
    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(QDir::homePath()));
    treeView->hideColumn(1); // Hide size column
    treeView->hideColumn(2); // Hide type column
    treeView->hideColumn(3); // Hide date column
    treeView->setMinimumWidth(250);
    
    // Create right panel widget
    QWidget *rightPanel = new QWidget(this);
    rightLayout = new QVBoxLayout(rightPanel);
    
    // Create horizontal splitter for the two list views
    rightSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Setup ListView 1
    QWidget *listWidget1 = new QWidget(this);
    QVBoxLayout *listLayout1 = new QVBoxLayout(listWidget1);
    QLabel *label1 = new QLabel("Original Files", this);
    label1->setStyleSheet("font-weight: bold; padding: 5px;");
    listView1 = new QListView(this);
    listView1->setModel(fileSystemModel);
    listView1->setRootIndex(fileSystemModel->index(QDir::homePath()));
    listLayout1->addWidget(label1);
    listLayout1->addWidget(listView1);
    
    // Setup ListView 2 with custom model
    QWidget *listWidget2 = new QWidget(this);
    QVBoxLayout *listLayout2 = new QVBoxLayout(listWidget2);
    QLabel *label2 = new QLabel("Modified Files", this);
    label2->setStyleSheet("font-weight: bold; padding: 5px;");
    customModel = new QStandardItemModel(this);
    listView2 = new QListView(this);
    listView2->setModel(customModel);
    listLayout2->addWidget(label2);
    listLayout2->addWidget(listView2);
    
    // Add list widgets to right splitter
    rightSplitter->addWidget(listWidget1);
    rightSplitter->addWidget(listWidget2);
    rightSplitter->setSizes({400, 400});
    
    // Setup command interface
    commandWidget = new QWidget(this);
    commandLayout = new QVBoxLayout(commandWidget);
    
    QWidget *commandInputWidget = new QWidget(this);
    QHBoxLayout *commandInputLayout = new QHBoxLayout(commandInputWidget);
    
    commandLabel = new QLabel("Command:", this);
    commandLineEdit = new QLineEdit(this);
    commandLineEdit->setPlaceholderText("Enter command (e.g., 'get filename')");
    commandLineEdit->setText("get filename"); // Default command
    executeButton = new QPushButton("Execute", this);
    
    commandInputLayout->addWidget(commandLabel);
    commandInputLayout->addWidget(commandLineEdit);
    commandInputLayout->addWidget(executeButton);
    
    commandLayout->addWidget(commandInputWidget);
    commandWidget->setMaximumHeight(60);
    
    // Add widgets to right layout
    rightLayout->addWidget(rightSplitter);
    rightLayout->addWidget(commandWidget);
    
    // Add panels to main splitter
    mainSplitter->addWidget(treeView);
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setSizes({300, 700});
    
    // Set main layout
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Connect signals
    connect(treeView, &QTreeView::clicked, this, &FileManager::onTreeViewClicked);
    connect(listView1, &QListView::clicked, this, &FileManager::onListView1Clicked);
    connect(executeButton, &QPushButton::clicked, this, &FileManager::onExecuteCommand);
    connect(commandLineEdit, &QLineEdit::returnPressed, this, &FileManager::onExecuteCommand);
    
    // Initialize ListView2 with current directory
    updateListView2();
}

void FileManager::onTreeViewClicked(const QModelIndex &index)
{
    if (fileSystemModel->isDir(index)) {
        currentPath = fileSystemModel->filePath(index);
        listView1->setRootIndex(index);
        updateListView2();
    }
}

void FileManager::onListView1Clicked(const QModelIndex &index)
{
    selectedFileIndex = index;
}

void FileManager::updateListView2()
{
    customModel->clear();
    customModel->setHorizontalHeaderLabels({"Modified Filename"});
    
    // Get current directory from ListView1
    QModelIndex currentIndex = listView1->rootIndex();
    if (!currentIndex.isValid()) {
        currentIndex = fileSystemModel->index(QDir::homePath());
    }
    
    // Populate ListView2 with files from current directory
    int rowCount = fileSystemModel->rowCount(currentIndex);
    for (int i = 0; i < rowCount; ++i) {
        QModelIndex childIndex = fileSystemModel->index(i, 0, currentIndex);
        if (fileSystemModel->isDir(childIndex)) {
            continue; // Skip directories for now
        }
        
        QString fileName = fileSystemModel->fileName(childIndex);
        QStandardItem *item = new QStandardItem(fileName);
        item->setData(fileSystemModel->filePath(childIndex), Qt::UserRole);
        customModel->appendRow(item);
    }
}

void FileManager::onExecuteCommand()
{
    QString command = commandLineEdit->text().trimmed().toLower();
    
    if (command == "get filename") {
        executeGetFilenameCommand();
    } else {
        QMessageBox::information(this, "Command Info", 
                                QString("Unknown command: %1\n\nAvailable commands:\n- get filename").arg(command));
    }
}

void FileManager::executeGetFilenameCommand()
{
    if (!selectedFileIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please select a file from the left list view first.");
        return;
    }
    
    // Get the selected filename from ListView1
    QString selectedFileName = fileSystemModel->fileName(selectedFileIndex);
    QString selectedFilePath = fileSystemModel->filePath(selectedFileIndex);
    
    if (selectedFileName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Could not get filename from selection.");
        return;
    }
    
    // Find corresponding item in ListView2 and update it
    bool found = false;
    for (int i = 0; i < customModel->rowCount(); ++i) {
        QStandardItem *item = customModel->item(i);
        if (item) {
            QString originalPath = item->data(Qt::UserRole).toString();
            if (originalPath == selectedFilePath) {
                // Update the filename in ListView2 to match ListView1
                item->setText(selectedFileName);
                found = true;
                break;
            }
        }
    }
    
    if (found) {
        QMessageBox::information(this, "Command Executed", 
                                QString("Filename updated in right list view: %1").arg(selectedFileName));
    } else {
        QMessageBox::warning(this, "Error", "Could not find corresponding file in right list view.");
    }
}
