# KIO Commander Development Proposals

## Date: 2025-06-18
## Status: PLANNING ONLY - NO IMPLEMENTATION UNTIL EXPLICITLY REQUESTED



 *   Directory button implementation proposal
 *   Visual focus indication proposal
 *   Additional splitter layout proposal
 *   Go To Tree buttons proposal
 *   Configuration system YAML structure
 *   Script execution system design
 *   Operation toggle system (for future)
 *   Minimal features list
 *   Architecture decisions from our discussion
 *   Your specified development priorities


### 1. Directory Button Implementation
```c++
// PROPOSAL: Add to FilePanel class
private:
    QPushButton *m_dirButton;

// PROPOSAL: In setupUI()
m_dirButton = new QPushButton(this);
m_dirButton->setFlat(true);
connect(m_dirButton, &QPushButton::clicked, this, &FilePanel::onDirButtonClicked);
m_layout->addWidget(m_dirButton);

// PROPOSAL: New slot
void FilePanel::onDirButtonClicked() {
    QMenu menu;
    menu.addAction("Up Directory", [this]() {
        QUrl parentUrl = m_currentDir.adjusted(QUrl::RemoveFilename);
        setDirectory(parentUrl);
    });
    menu.exec(QCursor::pos());
}

// PROPOSAL: Update button text in setDirectory()
m_dirButton->setText(url.fileName().isEmpty() ? "/" : url.fileName());
```

### 2. Visual Focus Indication
```c++
// PROPOSAL: Modify FocusFilter in main.cpp
class FocusFilter : public QObject {
public:
    FilePanel *&active;
    FilePanel *left;
    FilePanel *right;
    QString activeColor = "#3daee9";  // KDE blue
    QString inactiveColor = "#eff0f1"; // Gray
    
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::FocusIn) {
            if (obj == left || obj->parent() == left) {
                left->setStyleSheet(QString("FilePanel { border: 2px solid %1; }").arg(activeColor));
                right->setStyleSheet(QString("FilePanel { border: 2px solid %1; }").arg(inactiveColor));
                active = left;
            } else if (obj == right || obj->parent() == right) {
                right->setStyleSheet(QString("FilePanel { border: 2px solid %1; }").arg(activeColor));
                left->setStyleSheet(QString("FilePanel { border: 2px solid %1; }").arg(inactiveColor));
                active = right;
            }
        }
        return false;
    }
};
```

### 3. Additional Bottom Splitter
```c++
// PROPOSAL: New layout structure in main.cpp
// After existing vertical splitter setup:
QSplitter *bottomSplitter = new QSplitter(Qt::Vertical);
verticalSplitter->addWidget(bottomSplitter);

// Move file panels container to bottom splitter
bottomSplitter->addWidget(filePanelsContainer);

// Add empty bottom space
QWidget *bottomEmptyWidget = new QWidget;
bottomEmptyWidget->setStyleSheet("QWidget { background-color: #f5f5f5; }");
bottomSplitter->addWidget(bottomEmptyWidget);

// Set initial sizes
bottomSplitter->setSizes(QList<int>() << 400 << 200);
```

### 4. Go To Tree Buttons
```c++
// PROPOSAL: Above each FilePanel
QPushButton *goToTreeLeft = new QPushButton("Go To Tree");
connect(goToTreeLeft, &QPushButton::clicked, [treePanel, leftPanel]() {
    QModelIndex current = treePanel->currentIndex();
    if (current.isValid()) {
        QString path = treePanel->model()->filePath(current);
        leftPanel->setDirectory(QUrl::fromLocalFile(path));
    }
});

QPushButton *goToTreeRight = new QPushButton("Go To Tree");
connect(goToTreeRight, &QPushButton::clicked, [treePanel, rightPanel]() {
    QModelIndex current = treePanel->currentIndex();
    if (current.isValid()) {
        QString path = treePanel->model()->filePath(current);
        rightPanel->setDirectory(QUrl::fromLocalFile(path));
    }
});
```

### 5. Configuration System Structure (Future)
```yaml
# PROPOSAL: config.yaml structure
builtin_operations:
  copy:
    enabled: true
    fallback_script: "cp -r $SOURCE $DEST"
  move:
    enabled: true
    fallback_script: "mv $SOURCE $DEST"
  delete:
    enabled: true
    fallback_script: "rm -rf $SOURCE"
  rename:
    enabled: true
    fallback_script: "mv $SOURCE $DEST"

color_schemes:
  default:
    active_border: "#3daee9"
    inactive_border: "#eff0f1"
    background: "#ffffff"
  dark:
    active_border: "#1d99f3"
    inactive_border: "#31363b"
    background: "#232629"

layout:
  initial_sizes:
    tree_width: 300
    panels_width: 700
    top_height: 300
    panels_height: 300
  minimum_panel_height: 50  # percentage
```

### 6. Script Execution System (Future)
```c++
// PROPOSAL: ScriptExecutor class
class ScriptExecutor : public QObject {
    Q_OBJECT
public:
    void executeCommand(const QString &command, const QStringList &args) {
        QProcess *process = new QProcess(this);
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [process](int exitCode, QProcess::ExitStatus exitStatus) {
            qDebug() << "Command finished with code:" << exitCode;
            process->deleteLater();
        });
        process->start(command, args);
    }
    
    void executeYamlCommand(const QString &groupName, const QString &alias) {
        // Load from YAML config
        // Find command by group:alias
        // Execute with proper context (sudo, docker, etc.)
    }
};
```

### 7. Operation Toggle System (Future)
```c++
// PROPOSAL: Add to FilePanel when configuration system is ready
private:
    bool m_copyEnabled = true;
    bool m_moveEnabled = true;
    bool m_deleteEnabled = true;
    bool m_renameEnabled = true;

// PROPOSAL: Check in each operation
void FilePanel::onCopyFile() {
    if (!m_copyEnabled) {
        // Use script executor
        m_scriptExecutor->executeYamlCommand("file_ops", "copy");
        return;
    }
    // Existing KIO code...
}
```

### 8. Minimal Features for Development Continuation
1. Directory button with up navigation
2. Visual focus indication (colored borders)
3. Basic error dialogs instead of qDebug()
4. Simple YAML loading for testing scripting concepts

### 9. Architecture Decisions from Discussion
- **KISS Principle**: Keep it simple, no over-engineering
- **Button-Driven**: Explicit user actions, no automatic behaviors
- **Script-Focused**: Core functionality via YAML/shell scripts
- **KIO as Fallback**: Keep KIO operations, add toggles later
- **Minimal Predefined**: User configures everything
- **Power User Target**: Focus on flexibility over ease-of-use

### 10. Development Priorities (User Specified)
1. Light resource usage
2. Async capabilities
3. YAML scripting implementation
4. Minimal fixed buttons for testing
5. Configuration system before public alpha

---

## STATUS: ALL PROPOSALS - NO IMPLEMENTATION UNTIL REQUESTED

These proposals are saved for reference. Implementation will only occur when explicitly requested by the user with specific instructions on what to implement and when.
