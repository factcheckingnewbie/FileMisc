+// main.cpp - TreePanel refactor
+#include <QApplication>
+#include <QHBoxLayout>
+#include <QSplitter>
+#include <QDir>
+#include <QDebug>
+#include "FilePanel.h"
+#include "TreePanel.h"
+
+int main(int argc, char *argv[])
+{
+    qDebug() << "=== Application started ===";
+
+    QApplication app(argc, argv);
+
+    QWidget mainWin;
+    mainWin.setWindowTitle("KIO Commander");
+
+    QSplitter *splitter = new QSplitter(&mainWin);
+
+    // Use TreePanel instead of inline QTreeView/KDirModel
+    TreePanel *treePanel = new TreePanel;
+
+    FilePanel *leftPanel = new FilePanel;
+    FilePanel *rightPanel = new FilePanel;
+
+    // Start in home and root for demonstration; adapt as you wish.
+    leftPanel->setDirectory(QUrl::fromLocalFile(QDir::homePath()));
+    rightPanel->setDirectory(QUrl::fromLocalFile(QDir::rootPath()));
+
+    splitter->addWidget(treePanel);
+    splitter->addWidget(leftPanel);
+    splitter->addWidget(rightPanel);
+
+    QHBoxLayout *layout = new QHBoxLayout;
+    layout->addWidget(splitter);
+    mainWin.setLayout(layout);
+    mainWin.resize(1200, 600);
+    mainWin.show();
+
+    return app.exec();
+}
