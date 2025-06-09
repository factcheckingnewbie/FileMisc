#include "filemanager.h"
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QHeaderView>
#include <QEvent>
#include <QApplication>

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
    , fileSystemModel2(nullptr)
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
    
    // Setup independent file system model for listView2
    fileSystemModel2 = new QFileSystemModel(this);
    fileSystemModel2->setRootPath(QDir::rootPath());
    
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
    listView1->installEventFilter(this);
    listLayout1->addWidget(label1);
    listLayout1->addWidget(listView1);
    
    // Setup ListView 2 with independent file system model
    QWidget *listWidget2 = new QWidget(this);
    QVBoxLayout *listLayout2 = new QVBoxLayout(listWidget2);
    QLabel *label2 = new QLabel("Modified Files", this);
    label2->setStyleSheet("font-weight: bold; padding: 5px;");
    listView2 = new QListView(this);
    listView2->setModel(fileSystemModel2);
    listView2->setRootIndex(fileSystemModel2->index(QDir::homePath()));
    listView2->installEventFilter(this);
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
    connect(listView2, &QListView::clicked, this, &FileManager::onListView2Clicked);
    connect(executeButton, &QPushButton::clicked, this, &FileManager::onExecuteCommand);
    connect(commandLineEdit, &QLineEdit::returnPressed, this, &FileManager::onExecuteCommand);
    
    // Set initial focus styles
    updateFocusStyle();
}

void FileManager::onTreeViewClicked(const QModelIndex &index)
{
    if (fileSystemModel->isDir(index)) {
        currentPath = fileSystemModel->filePath(index);
        listView1->setRootIndex(index);
        // Removed synchronization - listView2 now operates independently
    }
}

void FileManager::onListView1Clicked(const QModelIndex &index)
{
    selectedFileIndex = index;
    updateFocusStyle();
}

void FileManager::onListView2Clicked(const QModelIndex &index)
{
    // Handle directory navigation for independent listView2
    if (fileSystemModel2->isDir(index)) {
        listView2->setRootIndex(index);
    }
    updateFocusStyle();
}

void FileManager::updateListView2()
{
    // This method is no longer used for synchronization
    // Keeping it for backward compatibility, but it does nothing
    // ListView2 now operates independently with its own QFileSystemModel
}

void FileManager::updateFocusStyle()
{
    // Apply visual styling based on which list view has focus
    QWidget *focusWidget = QApplication::focusWidget();
    
    // Define styles for focused and unfocused states
    QString focusedStyle = "QListView { border: 2px solid #4A90E2; background-color: #F0F8FF; }";
    QString unfocusedStyle = "QListView { border: 1px solid #C0C0C0; background-color: white; }";
    
    // Apply styles based on focus
    if (focusWidget == listView1) {
        listView1->setStyleSheet(focusedStyle);
        listView2->setStyleSheet(unfocusedStyle);
    } else if (focusWidget == listView2) {
        listView1->setStyleSheet(unfocusedStyle);
        listView2->setStyleSheet(focusedStyle);
    } else {
        // Default state - show listView1 as focused initially
        listView1->setStyleSheet(focusedStyle);
        listView2->setStyleSheet(unfocusedStyle);
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
    
    // Since ListView2 now uses QFileSystemModel instead of QStandardItemModel,
    // we'll just show the filename that would be used
    QMessageBox::information(this, "Command Executed", 
                            QString("Selected filename from left list view: %1\n\nNote: The right list view now operates independently and doesn't sync with the left one.").arg(selectedFileName));
}

bool FileManager::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusIn) {
        if (obj == listView1 || obj == listView2) {
            updateFocusStyle();
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
