#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QDebug>
#include <QListView>
#include "FilePanel.h"
#include "TreePanel.h"

int main(int argc, char *argv[])
{
    qDebug() << "=== Application started ===";

    QApplication app(argc, argv);

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    QSplitter *splitter = new QSplitter(&mainWin);

    TreePanel *treePanel = new TreePanel;
    FilePanel *leftPanel = new FilePanel;
    FilePanel *rightPanel = new FilePanel;

    // Start in home and root for demonstration
    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));

    splitter->addWidget(treePanel);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);

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

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);
    mainWin.setLayout(layout);
    mainWin.resize(1200, 600);
    mainWin.show();

    return app.exec();
}
