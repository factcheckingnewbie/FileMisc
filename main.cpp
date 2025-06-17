#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QDebug>
#include <QCommandLineParser>
#include <QFileInfo>
#include "FilePanel.h"
#include "TreePanel.h"
#include "PanelWrapper.h"

// FUTURE ActionMotor: This will be the central action processor
// class ActionMotor : public QObject {
//     Q_OBJECT
// public:
//     void registerActionHandler(const QString &actionPattern, std::function<void(const ActionRequest&)> handler);
//     void loadYamlConfiguration(const QString &path);
//     void executeAction(const ActionRequest &request);
// signals:
//     void actionCompleted(const QString &actionId, bool success, const QVariantMap &result);
// };

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

    // Create panels with explicit IDs for action routing
    TreePanel *treePanel = new TreePanel;
    PanelWrapper *leftPanelWrapper = new PanelWrapper("left-panel");
    PanelWrapper *rightPanelWrapper = new PanelWrapper("right-panel");
    
    // FUTURE ActionMotor: Create and configure
    // ActionMotor *actionMotor = new ActionMotor(&mainWin);
    // actionMotor->loadYamlConfiguration("~/.config/commandmaster/actions.yaml");
    
    // FUTURE ActionMotor: Register action handlers
    // actionMotor->registerActionHandler("navigation:syncTreeToPanel", [=](const ActionRequest &req) {
    //     QUrl targetUrl(req.parameters["targetUrl"].toString());
    //     treePanel->navigateToPath(targetUrl);
    // });
    
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

    // Set initial directories
    leftPanelWrapper->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanelWrapper->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    // Add widgets to splitter
    splitter->addWidget(treePanel);
    splitter->addWidget(leftPanelWrapper);
    splitter->addWidget(rightPanelWrapper);

    // SIGNALS-ONLY APPROACH: Multiple independent signal handlers
    
    // Connect panel "Go To Tree" requests to TreePanel
    QObject::connect(leftPanelWrapper, &PanelWrapper::goToTreeRequested,
        [treePanel](const QUrl &url, const QString &panelId) {
            qDebug() << "[SIGNAL] Panel" << panelId << "requested tree navigation to:" << url.toString();
            treePanel->navigateToPath(url);
        });
    
    QObject::connect(rightPanelWrapper, &PanelWrapper::goToTreeRequested,
        [treePanel](const QUrl &url, const QString &panelId) {
            qDebug() << "[SIGNAL] Panel" << panelId << "requested tree navigation to:" << url.toString();
            treePanel->navigateToPath(url);
        });
    
    // MULTIPLE SIGNAL HANDLERS: Each panel has its own connection to TreePanel
    // This allows independent control of which panels follow TreePanel
    
    // Connect TreePanel to LEFT panel (independent connection)
    QObject::connect(treePanel, &TreePanel::directorySelected,
        leftPanelWrapper, &PanelWrapper::onTreePanelDirectorySelected);
    
    // Connect TreePanel to RIGHT panel (independent connection)
    QObject::connect(treePanel, &TreePanel::directorySelected,
        rightPanelWrapper, &PanelWrapper::onTreePanelDirectorySelected);
    
    // Set initial follows-tree state
    leftPanelWrapper->setFollowsTreePanel(true);   // Left panel follows tree by default
    rightPanelWrapper->setFollowsTreePanel(false); // Right panel doesn't follow by default
    
    // Log when panels toggle their follow state
    QObject::connect(leftPanelWrapper, &PanelWrapper::followsTreePanelToggled,
        [](bool follows, const QString &panelId) {
            qDebug() << "[TOGGLE] Panel" << panelId 
                     << (follows ? "now follows" : "stopped following") 
                     << "TreePanel";
        });
    
    QObject::connect(rightPanelWrapper, &PanelWrapper::followsTreePanelToggled,
        [](bool follows, const QString &panelId) {
            qDebug() << "[TOGGLE] Panel" << panelId 
                     << (follows ? "now follows" : "stopped following") 
                     << "TreePanel";
        });
    
    // FUTURE CommandMaster: Connect action requests to ActionMotor
    // QObject::connect(leftPanelWrapper, &PanelWrapper::actionRequested,
    //     [=](const QString &actionId, const QVariantMap &params, const QString &panelId) {
    //         ActionRequest req;
    //         req.actionId = actionId;
    //         req.panelId = panelId;
    //         req.parameters = params;
    //         req.timestamp = QDateTime::currentDateTime();
    //         req.user = qgetenv("USER");
    //         actionMotor->executeAction(req);
    //     });
    
    // FUTURE Audit: Connect auditable actions to logging system
    // QObject::connect(leftPanelWrapper, &PanelWrapper::auditableActionPerformed,
    //     [=](const QString &action, const QVariantMap &details, const QString &panelId) {
    //         qDebug() << "[AUDIT]" << QDateTime::currentDateTime().toString()
    //                  << "Panel:" << panelId
    //                  << "Action:" << action
    //                  << "Details:" << details;
    //         // Write to audit log file
    //     });

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);
    mainWin.setLayout(layout);
    mainWin.resize(1200, 600);
    mainWin.show();

    return app.exec();
}

// FUTURE CommandMaster: Example YAML structure that ActionMotor would load
/*
actions:
  - id: "navigation:syncTreeToPanel"
    label: "Sync Tree to Panel"
    description: "Navigate tree view to current panel directory"
    category: "navigation"
    protected: false
    parameters:
      - name: "targetUrl"
        type: "url"
        required: true
    
  - id: "command:execute"
    label: "Execute Command"
    description: "Execute a shell command"
    category: "system"
    protected: true
    contexts: ["plain", "sudo", "docker"]
    parameters:
      - name: "command"
        type: "string"
        required: true
      - name: "workingDirectory"
        type: "url"
        required: false
        
  - id: "file:copy"
    label: "Copy Files"
    description: "Copy files from source to destination"
    category: "file"
    protected: false
    parameters:
      - name: "source"
        type: "url"
        required: true
      - name: "destination"
        type: "url"
        required: true
      - name: "overwrite"
        type: "boolean"
        default: false
*/
