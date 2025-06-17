#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include "FilePanel.h"
#include "TreePanel.h"

int main(int argc, char *argv[])
{
    qDebug() << "=== Application started ===";

    QApplication app(argc, argv);
    app.setApplicationName("FileMisc");
    app.setApplicationDisplayName("KIO Commander");

    // Set up command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("File manager with tree view navigation");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add positional argument for path
    parser.addPositionalArgument("path", "Directory path to open");
    
    // Process the command line arguments
    parser.process(app);
    
    // Get the path argument if provided
    const QStringList args = parser.positionalArguments();
    
    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    QSplitter *splitter = new QSplitter(&mainWin);

    TreePanel *treePanel = new TreePanel;
    FilePanel *leftPanel = new FilePanel;
    FilePanel *rightPanel = new FilePanel;
    
    // Navigate TreePanel based on command line argument
    if (!args.isEmpty()) {
        QString pathArg = args.first();
        
        // Convert to absolute path
        QString absolutePath = QFileInfo(pathArg).absoluteFilePath();
        QFileInfo fileInfo(absolutePath);
        
        qDebug() << "Path argument:" << pathArg;
        qDebug() << "Absolute path:" << absolutePath;
        
        if (fileInfo.exists() && fileInfo.isDir()) {
            QUrl targetUrl = QUrl::fromLocalFile(absolutePath);
            qDebug() << "Valid directory, navigating TreePanel to:" << targetUrl.toString();
            treePanel->navigateToPath(targetUrl);
        } else {
            qDebug() << "WARNING: Not a valid directory path:" << absolutePath;
            qDebug() << "TreePanel will navigate to home directory instead";
            treePanel->navigateToPath(QUrl::fromLocalFile(QDir::homePath()));
        }
    } else {
        // No path provided - navigate to home directory
        qDebug() << "No path provided, navigating TreePanel to home directory";
        treePanel->navigateToPath(QUrl::fromLocalFile(QDir::homePath()));
    }

    // Keep original FilePanel initialization (NOT MODIFIED)
    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    splitter->addWidget(treePanel);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);

    // Directory navigation: Connect TreePanel to left FilePanel
    QObject::connect(treePanel, &TreePanel::directorySelected,
                     leftPanel, &FilePanel::setDirectory);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);
    mainWin.setLayout(layout);
    mainWin.resize(1200, 600);
    mainWin.show();

    return app.exec();
}
