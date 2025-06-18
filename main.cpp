#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QDebug>
#include <QListView>
#include <QWidget>
#include <QFrame>
#include "FilePanel.h"
#include "TreePanel.h"

int main(int argc, char *argv[])
{
    qDebug() << "=== Application started ===";

    QApplication app(argc, argv);

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    // Create main vertical layout for the window
    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWin);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create top panel (future menubar)
    QFrame *topPanel = new QFrame;
    topPanel->setFrameStyle(QFrame::Box | QFrame::Raised);
    topPanel->setMinimumHeight(30);
    topPanel->setMaximumHeight(30);
    topPanel->setStyleSheet("QFrame { background-color: #f0f0f0; }");
    mainLayout->addWidget(topPanel);

    // Create horizontal splitter for tree and right side
    QSplitter *horizontalSplitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(horizontalSplitter);

    // Create tree panel (left side, full height)
    TreePanel *treePanel = new TreePanel;
    horizontalSplitter->addWidget(treePanel);

    // Create vertical splitter for right side
    QSplitter *verticalSplitter = new QSplitter(Qt::Vertical);
    horizontalSplitter->addWidget(verticalSplitter);

    // Create empty widget for top part of vertical splitter
    QWidget *topEmptyWidget = new QWidget;
    topEmptyWidget->setStyleSheet("QWidget { background-color: #fafafa; }");
    verticalSplitter->addWidget(topEmptyWidget);

    // Create container for the two file panels
    QWidget *filePanelsContainer = new QWidget;
    QHBoxLayout *filePanelsLayout = new QHBoxLayout(filePanelsContainer);
    filePanelsLayout->setContentsMargins(0, 0, 0, 0);
    filePanelsLayout->setSpacing(2);

    // Create file panels
    FilePanel *leftPanel = new FilePanel;
    FilePanel *rightPanel = new FilePanel;

    // Set initial directories
    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    // Add file panels to container
    filePanelsLayout->addWidget(leftPanel);
    filePanelsLayout->addWidget(rightPanel);

    // Add file panels container to vertical splitter
    verticalSplitter->addWidget(filePanelsContainer);

    // Set initial splitter sizes
    // Horizontal splitter: 30% tree, 70% right side
    horizontalSplitter->setSizes(QList<int>() << 300 << 700);
    
    // Vertical splitter: 50% empty top, 50% file panels (minimum for file panels)
    verticalSplitter->setSizes(QList<int>() << 300 << 300);

    // CONSTRAINT: Ensure file panels never go below 50% of vertical splitter height
    // Connect to splitter movement to enforce minimum
    QObject::connect(verticalSplitter, &QSplitter::splitterMoved, 
                     [verticalSplitter](int pos, int index) {
        QList<int> sizes = verticalSplitter->sizes();
        int totalHeight = sizes[0] + sizes[1];
        int minFilePanelHeight = totalHeight / 2;  // 50% minimum
        
        // If file panels (bottom) would be less than 50%, adjust
        if (sizes[1] < minFilePanelHeight) {
            sizes[0] = totalHeight - minFilePanelHeight;
            sizes[1] = minFilePanelHeight;
            verticalSplitter->setSizes(sizes);
            qDebug() << "Enforced minimum 50% height for file panels";
        }
        
        qDebug() << "Vertical splitter sizes - Top:" << sizes[0] << "Bottom:" << sizes[1];
    });

    // Track active panel
    FilePanel *activePanel = leftPanel;
    
    class FocusFilter : public QObject {
    public:
        FilePanel *&active;
        FilePanel *left;
        FilePanel *right;
        
        FocusFilter(FilePanel *&activeRef, FilePanel *l, FilePanel *r, QObject *parent = nullptr) 
            : QObject(parent), active(activeRef), left(l), right(r) {}
        
        bool eventFilter(QObject *obj, QEvent *event) override {
            if (event->type() == QEvent::FocusIn) {
                if (obj == left || obj->parent() == left) {
                    qDebug() << "Left panel activated";
                    active = left;
                } else if (obj == right || obj->parent() == right) {
                    qDebug() << "Right panel activated";
                    active = right;
                }
            }
            return false;
        }
    };
    
    // Create filter with mainWin as parent (auto-deleted)
    FocusFilter *filter = new FocusFilter(activePanel, leftPanel, rightPanel, &mainWin);
    
    leftPanel->installEventFilter(filter);
    rightPanel->installEventFilter(filter);
    
    // Safely install on list views if they exist
    if (QListView *leftList = leftPanel->findChild<QListView*>()) {
        leftList->installEventFilter(filter);
    }
    if (QListView *rightList = rightPanel->findChild<QListView*>()) {
        rightList->installEventFilter(filter);
    }
    
    // Connect TreePanel to update active panel
    QObject::connect(treePanel, &TreePanel::directorySelected,
                     [&activePanel](const QUrl &url) {
                         if (activePanel) {
                             qDebug() << "TreePanel updating active panel to:" << url.toString();
                             activePanel->setDirectory(url);
                         }
                     });

    mainWin.resize(1200, 600);
    mainWin.show();

    return app.exec();
}
