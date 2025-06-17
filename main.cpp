#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include "FilePanel.h"
#include "TreePanel.h"

int main(int argc, char *argv[])
{
    qDebug() << "=== Application started ===";

    QApplication app(argc, argv);

    QWidget mainWin;
    mainWin.setWindowTitle("KIO Commander");

    QSplitter *splitter = new QSplitter(&mainWin);
    if (!splitter) {
        qDebug() << "[ERROR] Failed to create QSplitter";
        return 1;
    }

    TreePanel *treePanel = nullptr;
    try {
        treePanel = new TreePanel;
        if (!treePanel) {
            qDebug() << "[ERROR] Failed to create TreePanel";
            return 1;
        }
    } catch (const std::exception& e) {
        qDebug() << "[ERROR] Exception creating TreePanel:" << e.what();
        return 1;
    } catch (...) {
        qDebug() << "[ERROR] Unknown exception creating TreePanel";
        return 1;
    }
    
    // Create left panel with button
    QWidget *leftContainer = nullptr;
    FilePanel *leftPanel = nullptr;
    QPushButton *leftGoToTreeBtn = nullptr;
    try {
        leftContainer = new QWidget;
        if (!leftContainer) {
            qDebug() << "[ERROR] Failed to create left container";
            delete treePanel;
            return 1;
        }
        
        QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
        if (!leftLayout) {
            qDebug() << "[ERROR] Failed to create left layout";
            delete leftContainer;
            delete treePanel;
            return 1;
        }
        
        leftPanel = new FilePanel;
        if (!leftPanel) {
            qDebug() << "[ERROR] Failed to create left FilePanel";
            delete leftContainer;
            delete treePanel;
            return 1;
        }
        
        leftGoToTreeBtn = new QPushButton("Go to tree");
        if (!leftGoToTreeBtn) {
            qDebug() << "[ERROR] Failed to create left button";
            delete leftPanel;
            delete leftContainer;
            delete treePanel;
            return 1;
        }
        
        leftLayout->addWidget(leftGoToTreeBtn);
        leftLayout->addWidget(leftPanel);
        leftLayout->setContentsMargins(0, 0, 0, 0);
        qDebug() << "[DEBUG] Left panel container created successfully";
        
    } catch (const std::exception& e) {
        qDebug() << "[ERROR] Exception creating left panel:" << e.what();
        // Clean up any allocated objects
        delete leftGoToTreeBtn;
        delete leftPanel;
        delete leftContainer;
        delete treePanel;
        return 1;
    } catch (...) {
        qDebug() << "[ERROR] Unknown exception creating left panel";
        delete leftGoToTreeBtn;
        delete leftPanel;
        delete leftContainer;
        delete treePanel;
        return 1;
    }
    
    // Create right panel with button
    QWidget *rightContainer = nullptr;
    FilePanel *rightPanel = nullptr;
    QPushButton *rightGoToTreeBtn = nullptr;
    try {
        rightContainer = new QWidget;
        if (!rightContainer) {
            qDebug() << "[ERROR] Failed to create right container";
            delete leftContainer;
            delete treePanel;
            return 1;
        }
        
        QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
        if (!rightLayout) {
            qDebug() << "[ERROR] Failed to create right layout";
            delete rightContainer;
            delete leftContainer;
            delete treePanel;
            return 1;
        }
        
        rightPanel = new FilePanel;
        if (!rightPanel) {
            qDebug() << "[ERROR] Failed to create right FilePanel";
            delete rightContainer;
            delete leftContainer;
            delete treePanel;
            return 1;
        }
        
        rightGoToTreeBtn = new QPushButton("Go to tree");
        if (!rightGoToTreeBtn) {
            qDebug() << "[ERROR] Failed to create right button";
            delete rightPanel;
            delete rightContainer;
            delete leftContainer;
            delete treePanel;
            return 1;
        }
        
        rightLayout->addWidget(rightGoToTreeBtn);
        rightLayout->addWidget(rightPanel);
        rightLayout->setContentsMargins(0, 0, 0, 0);
        qDebug() << "[DEBUG] Right panel container created successfully";
        
    } catch (const std::exception& e) {
        qDebug() << "[ERROR] Exception creating right panel:" << e.what();
        // Clean up any allocated objects
        delete rightGoToTreeBtn;
        delete rightPanel;
        delete rightContainer;
        delete leftContainer;
        delete treePanel;
        return 1;
    } catch (...) {
        qDebug() << "[ERROR] Unknown exception creating right panel";
        delete rightGoToTreeBtn;
        delete rightPanel;
        delete rightContainer;
        delete leftContainer;
        delete treePanel;
        return 1;
    }

    // Start in home and root for demonstration; adapt as you wish.
    if (leftPanel) {
        leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
        qDebug() << "[DEBUG] Left panel directory set to:" << QDir::homePath();
    }
    if (rightPanel) {
        rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));
        qDebug() << "[DEBUG] Right panel directory set to:" << QDir::rootPath();
    }

    splitter->addWidget(treePanel);
    splitter->addWidget(leftContainer);
    splitter->addWidget(rightContainer);
    qDebug() << "[DEBUG] All widgets added to splitter";

    // Left panel "Go to tree" button
    QObject::connect(leftGoToTreeBtn, &QPushButton::clicked, &mainWin, [treePanel, leftPanel]() {
        qDebug() << "[DEBUG] Left 'Go to tree' button clicked";
        
        if (!treePanel) {
            qDebug() << "[ERROR] treePanel is null in left button handler";
            return;
        }
        
        QTreeView* treeView = treePanel->view();
        if (!treeView) {
            qDebug() << "[ERROR] treePanel->view() returned null in left button handler";
            return;
        }
        
        QModelIndex currentIndex = treeView->currentIndex();
        if (currentIndex.isValid()) {
            qDebug() << "[DEBUG] Current tree index is valid";
            
            KDirModel* model = treePanel->model();
            if (!model) {
                qDebug() << "[ERROR] treePanel->model() returned null in left button handler";
                return;
            }
            
            // Check if this is an expanded directory (not a collapsed node)
            bool isExpanded = treeView->isExpanded(currentIndex);
            bool hasChildren = model->hasChildren(currentIndex);
            qDebug() << "[DEBUG] Node state - isExpanded:" << isExpanded << "hasChildren:" << hasChildren;
            
            if (isExpanded || !hasChildren) {
                try {
                    KFileItem item = model->itemForIndex(currentIndex);
                    if (!item.isNull()) {
                        if (item.isDir()) {
                            if (leftPanel) {
                                leftPanel->setDirectory(item.url());
                                qDebug() << "[DEBUG] Left panel navigated to:" << item.url().toString();
                            } else {
                                qDebug() << "[ERROR] leftPanel is null when trying to set directory";
                            }
                        } else {
                            qDebug() << "[DEBUG] Selected item is not a directory";
                        }
                    } else {
                        qDebug() << "[ERROR] itemForIndex returned null KFileItem";
                    }
                } catch (const std::exception& e) {
                    qDebug() << "[ERROR] Exception in left button handler:" << e.what();
                } catch (...) {
                    qDebug() << "[ERROR] Unknown exception in left button handler";
                }
            } else {
                qDebug() << "[DEBUG] Cannot navigate to collapsed node";
            }
        } else {
            qDebug() << "[DEBUG] No valid selection in tree view";
        }
    });

    // Right panel "Go to tree" button
    QObject::connect(rightGoToTreeBtn, &QPushButton::clicked, &mainWin, [treePanel, rightPanel]() {
        qDebug() << "[DEBUG] Right 'Go to tree' button clicked";
        
        if (!treePanel) {
            qDebug() << "[ERROR] treePanel is null in right button handler";
            return;
        }
        
        QTreeView* treeView = treePanel->view();
        if (!treeView) {
            qDebug() << "[ERROR] treePanel->view() returned null in right button handler";
            return;
        }
        
        QModelIndex currentIndex = treeView->currentIndex();
        if (currentIndex.isValid()) {
            qDebug() << "[DEBUG] Current tree index is valid";
            
            KDirModel* model = treePanel->model();
            if (!model) {
                qDebug() << "[ERROR] treePanel->model() returned null in right button handler";
                return;
            }
            
            // Check if this is an expanded directory (not a collapsed node)
            bool isExpanded = treeView->isExpanded(currentIndex);
            bool hasChildren = model->hasChildren(currentIndex);
            qDebug() << "[DEBUG] Node state - isExpanded:" << isExpanded << "hasChildren:" << hasChildren;
            
            if (isExpanded || !hasChildren) {
                try {
                    KFileItem item = model->itemForIndex(currentIndex);
                    if (!item.isNull()) {
                        if (item.isDir()) {
                            if (rightPanel) {
                                rightPanel->setDirectory(item.url());
                                qDebug() << "[DEBUG] Right panel navigated to:" << item.url().toString();
                            } else {
                                qDebug() << "[ERROR] rightPanel is null when trying to set directory";
                            }
                        } else {
                            qDebug() << "[DEBUG] Selected item is not a directory";
                        }
                    } else {
                        qDebug() << "[ERROR] itemForIndex returned null KFileItem";
                    }
                } catch (const std::exception& e) {
                    qDebug() << "[ERROR] Exception in right button handler:" << e.what();
                } catch (...) {
                    qDebug() << "[ERROR] Unknown exception in right button handler";
                }
            } else {
                qDebug() << "[DEBUG] Cannot navigate to collapsed node";
            }
        } else {
            qDebug() << "[DEBUG] No valid selection in tree view";
        }
    });

    QHBoxLayout *layout = new QHBoxLayout;
    if (!layout) {
        qDebug() << "[ERROR] Failed to create main layout";
        return 1;
    }
    
    layout->addWidget(splitter);
    mainWin.setLayout(layout);
    mainWin.resize(1200, 600);
    mainWin.show();

    qDebug() << "[DEBUG] Main window configured and shown";

    return app.exec();
}
