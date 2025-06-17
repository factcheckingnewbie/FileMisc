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

    // SIGNALS-ONLY APPROACH: No direct object references
    // Each component communicates only through signals
    
    // Connect panel "Go To Tree" requests to TreePanel
    QObject::connect(leftPanelWrapper, &PanelWrapper::goToTreeRequested,
        [=](const QUrl &url, const QString &panelId) {
            qDebug() << "[SIGNAL] Panel" << panelId << "requested tree navigation to:" << url.toString();
            // FUTURE Audit: This would go through ActionMotor for logging
            treePanel->navigateToPath(url);
        });
    
    QObject::connect(rightPanelWrapper, &PanelWrapper::goToTreeRequested,
        [=](const QUrl &url, const QString &panelId) {
            qDebug() << "[SIGNAL] Panel" << panelId << "requested tree navigation to:" << url.toString();
            // FUTURE Audit: This would go through ActionMotor for logging
            treePanel->navigateToPath(url);
        });
    
    // IMPORTANT: Track which panel should receive TreePanel updates
    // This replaces the old behavior where only left panel was connected
    PanelWrapper *activePanel = nullptr;
    
    // Lambda to update active panel and visual feedback
    auto setActivePanel = [&](PanelWrapper *panel) {
        // Remove highlight from previous active panel
        if (activePanel) {
            activePanel->setStyleSheet("");
        }
        
        // Set new active panel
        activePanel = panel;
        
        // Add visual highlight to active panel
        if (activePanel) {
            activePanel->setStyleSheet("PanelWrapper { border: 2px solid #3daee9; }");
            qDebug() << "[ACTIVE] Panel" << activePanel->panelId() << "is now active for TreePanel updates";
        }
    };
    
    // Connect mouse events to track active panel
    // When user clicks on a panel, it becomes active for TreePanel updates
    leftPanelWrapper->installEventFilter(new QObject);
    rightPanelWrapper->installEventFilter(new QObject);
    
    class PanelClickFilter : public QObject {
    public:
        PanelWrapper *panel;
        std::function<void()> callback;
        
        PanelClickFilter(PanelWrapper *p, std::function<void()> cb) : panel(p), callback(cb) {}
        
        bool eventFilter(QObject *obj, QEvent *event) override {
            if (event->type() == QEvent::MouseButtonPress) {
                callback();
            }
            return false; // Don't consume the event
        }
    };
    
    // Install click detection on panels
    leftPanelWrapper->installEventFilter(new PanelClickFilter(leftPanelWrapper, [&]() {
        setActivePanel(leftPanelWrapper);
    }));
    
    rightPanelWrapper->installEventFilter(new PanelClickFilter(rightPanelWrapper, [&]() {
        setActivePanel(rightPanelWrapper);
    }));
    
    // Connect TreePanel directory selection to ACTIVE panel
    QObject::connect(treePanel, &TreePanel::directorySelected,
        [=, &activePanel](const QUrl &url) {
            qDebug() << "[SIGNAL] TreePanel selected directory:" << url.toString();
            
            if (activePanel) {
                qDebug() << "[SIGNAL] Updating active panel:" << activePanel->panelId();
                activePanel->setDirectory(url);
            } else {
                qDebug() << "[SIGNAL] No active panel - TreePanel selection ignored";
            }
            
            // FUTURE ActionMotor: This would become an action request
            // ActionRequest req;
            // req.actionId = "navigation:updatePanel";
            // req.parameters["targetUrl"] = url.toString();
            // req.parameters["targetPanel"] = activePanel ? activePanel->panelId() : "none";
            // actionMotor->executeAction(req);
        });
    
    // Set left panel as initially active (matching old behavior)
    setActivePanel(leftPanelWrapper);
    
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
