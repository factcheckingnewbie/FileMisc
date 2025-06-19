# CommandMaster UI-Complex System

## Overview
The UI-Complex is a comprehensive system for dynamic UI generation in CommandMaster, allowing scripts to create panels, menus, buttons, and any Qt widget on demand.

## Core Components

### 1. Widget Factory
```c++
class WidgetFactory : public QObject {
    Q_OBJECT
    
private:
    QMap<QString, QWidget*> m_widgets;
    QMainWindow *m_mainWindow;
    
public:
    WidgetFactory(QMainWindow *parent) : QObject(parent), m_mainWindow(parent) {}
    
    // Create any Qt widget by type name
    QWidget* createWidget(const QString &type, const QString &id, QWidget *parent = nullptr) {
        QWidget *widget = nullptr;
        
        if (type == "QListView") {
            widget = new QListView(parent);
        } else if (type == "QPushButton") {
            widget = new QPushButton(parent);
        } else if (type == "QTextEdit") {
            widget = new QTextEdit(parent);
        } else if (type == "QLineEdit") {
            widget = new QLineEdit(parent);
        } else if (type == "QTreeView") {
            widget = new QTreeView(parent);
        } else if (type == "QTableView") {
            widget = new QTableView(parent);
        } else if (type == "QLabel") {
            widget = new QLabel(parent);
        } else if (type == "QComboBox") {
            widget = new QComboBox(parent);
        } else if (type == "QCheckBox") {
            widget = new QCheckBox(parent);
        } else if (type == "QRadioButton") {
            widget = new QRadioButton(parent);
        } else if (type == "QSlider") {
            widget = new QSlider(parent);
        } else if (type == "QProgressBar") {
            widget = new QProgressBar(parent);
        } else if (type == "QToolBar") {
            widget = new QToolBar(parent);
        } else if (type == "QMenuBar") {
            widget = new QMenuBar(parent);
        } else if (type == "QStatusBar") {
            widget = new QStatusBar(parent);
        } else if (type == "QDockWidget") {
            widget = new QDockWidget(parent);
        } else if (type == "QTabWidget") {
            widget = new QTabWidget(parent);
        } else if (type == "QSplitter") {
            widget = new QSplitter(parent);
        } else if (type == "QGroupBox") {
            widget = new QGroupBox(parent);
        } else if (type == "QFrame") {
            widget = new QFrame(parent);
        } else {
            qDebug() << "Unknown widget type:" << type;
            return nullptr;
        }
        
        widget->setObjectName(id);
        m_widgets[id] = widget;
        return widget;
    }
    
    QWidget* getWidget(const QString &id) {
        return m_widgets.value(id, nullptr);
    }
    
    void deleteWidget(const QString &id) {
        if (m_widgets.contains(id)) {
            m_widgets[id]->deleteLater();
            m_widgets.remove(id);
        }
    }
};
```

### 2. Panel Manager
```c++
class PanelManager : public QObject {
    Q_OBJECT
    
private:
    QMainWindow *m_mainWindow;
    WidgetFactory *m_factory;
    QMap<QString, QDockWidget*> m_dockPanels;
    
public:
    PanelManager(QMainWindow *mainWindow, WidgetFactory *factory) 
        : QObject(mainWindow), m_mainWindow(mainWindow), m_factory(factory) {}
    
    // Create floating or docked panel
    QWidget* createPanel(const QString &id, const QString &title, 
                        const QString &position = "float", 
                        int width = 400, int height = 300) {
        
        if (position == "float") {
            // Create floating window
            QWidget *panel = new QWidget(nullptr, Qt::Window);
            panel->setWindowTitle(title);
            panel->setObjectName(id);
            panel->resize(width, height);
            panel->show();
            return panel;
            
        } else {
            // Create dock widget
            QDockWidget *dock = new QDockWidget(title, m_mainWindow);
            dock->setObjectName(id);
            
            QWidget *content = new QWidget;
            dock->setWidget(content);
            
            Qt::DockWidgetArea area = Qt::BottomDockWidgetArea;
            if (position == "left") area = Qt::LeftDockWidgetArea;
            else if (position == "right") area = Qt::RightDockWidgetArea;
            else if (position == "top") area = Qt::TopDockWidgetArea;
            
            m_mainWindow->addDockWidget(area, dock);
            m_dockPanels[id] = dock;
            
            return content;
        }
    }
    
    // Add panel to splitter
    void addToSplitter(const QString &splitterId, const QString &widgetId) {
        QSplitter *splitter = qobject_cast<QSplitter*>(m_factory->getWidget(splitterId));
        QWidget *widget = m_factory->getWidget(widgetId);
        
        if (splitter && widget) {
            splitter->addWidget(widget);
        }
    }
    
    // Create panel with specific layout
    QLayout* createPanelLayout(const QString &panelId, const QString &layoutType) {
        QWidget *panel = m_factory->getWidget(panelId);
        if (!panel) return nullptr;
        
        QLayout *layout = nullptr;
        if (layoutType == "vertical") {
            layout = new QVBoxLayout(panel);
        } else if (layoutType == "horizontal") {
            layout = new QHBoxLayout(panel);
        } else if (layoutType == "grid") {
            layout = new QGridLayout(panel);
        } else if (layoutType == "form") {
            layout = new QFormLayout(panel);
        }
        
        return layout;
    }
};
```

### 3. Menu Builder
```c++
class MenuBuilder : public QObject {
    Q_OBJECT
    
private:
    QMainWindow *m_mainWindow;
    QMap<QString, QMenu*> m_menus;
    QMap<QString, QAction*> m_actions;
    
public:
    MenuBuilder(QMainWindow *mainWindow) : QObject(mainWindow), m_mainWindow(mainWindow) {}
    
    // Create menu
    QMenu* createMenu(const QString &id, const QString &title, const QString &parentId = "") {
        QMenu *menu = nullptr;
        
        if (parentId.isEmpty()) {
            // Top-level menu
            menu = m_mainWindow->menuBar()->addMenu(title);
        } else if (m_menus.contains(parentId)) {
            // Submenu
            menu = m_menus[parentId]->addMenu(title);
        } else {
            // Context menu
            menu = new QMenu(title, m_mainWindow);
        }
        
        menu->setObjectName(id);
        m_menus[id] = menu;
        return menu;
    }
    
    // Add action to menu
    QAction* addAction(const QString &menuId, const QString &actionId, 
                      const QString &text, const QString &shortcut = "",
                      const QString &icon = "") {
        
        QMenu *menu = m_menus.value(menuId);
        if (!menu) return nullptr;
        
        QAction *action = new QAction(text, menu);
        action->setObjectName(actionId);
        
        if (!shortcut.isEmpty()) {
            action->setShortcut(QKeySequence(shortcut));
        }
        
        if (!icon.isEmpty()) {
            action->setIcon(QIcon(icon));
        }
        
        menu->addAction(action);
        m_actions[actionId] = action;
        
        return action;
    }
    
    // Add separator
    void addSeparator(const QString &menuId) {
        if (QMenu *menu = m_menus.value(menuId)) {
            menu->addSeparator();
        }
    }
    
    // Show context menu
    void showContextMenu(const QString &menuId, int x, int y) {
        if (QMenu *menu = m_menus.value(menuId)) {
            menu->exec(QPoint(x, y));
        }
    }
};
```

### 4. Button Creator
```c++
class ButtonCreator : public QObject {
    Q_OBJECT
    
private:
    WidgetFactory *m_factory;
    
public:
    ButtonCreator(WidgetFactory *factory) : QObject(factory), m_factory(factory) {}
    
    // Create button with all properties
    QPushButton* createButton(const QString &id, const QString &text,
                             QWidget *parent = nullptr,
                             const QString &icon = "",
                             const QString &tooltip = "",
                             const QString &style = "") {
        
        QPushButton *button = qobject_cast<QPushButton*>(
            m_factory->createWidget("QPushButton", id, parent)
        );
        
        if (!button) return nullptr;
        
        button->setText(text);
        
        if (!icon.isEmpty()) {
            button->setIcon(QIcon(icon));
        }
        
        if (!tooltip.isEmpty()) {
            button->setToolTip(tooltip);
        }
        
        if (!style.isEmpty()) {
            button->setStyleSheet(style);
        }
        
        return button;
    }
    
    // Create toolbar button
    QAction* createToolButton(QToolBar *toolbar, const QString &id,
                             const QString &text, const QString &icon,
                             const QString &tooltip = "") {
        
        QAction *action = new QAction(text, toolbar);
        action->setObjectName(id);
        
        if (!icon.isEmpty()) {
            action->setIcon(QIcon(icon));
        }
        
        if (!tooltip.isEmpty()) {
            action->setToolTip(tooltip);
        }
        
        toolbar->addAction(action);
        return action;
    }
    
    // Create button group
    void createButtonGroup(QWidget *parent, const QStringList &buttons,
                          const QString &layout = "horizontal") {
        
        QLayout *buttonLayout = nullptr;
        if (layout == "horizontal") {
            buttonLayout = new QHBoxLayout(parent);
        } else {
            buttonLayout = new QVBoxLayout(parent);
        }
        
        for (const QString &buttonText : buttons) {
            QString id = buttonText.toLower().replace(" ", "_");
            QPushButton *btn = createButton(id, buttonText, parent);
            buttonLayout->addWidget(btn);
        }
    }
};
```

### 5. UI Script Interface
```c++
class UIScriptInterface : public QObject {
    Q_OBJECT
    
private:
    WidgetFactory *m_widgetFactory;
    PanelManager *m_panelManager;
    MenuBuilder *m_menuBuilder;
    ButtonCreator *m_buttonCreator;
    
public:
    UIScriptInterface(QMainWindow *mainWindow) : QObject(mainWindow) {
        m_widgetFactory = new WidgetFactory(mainWindow);
        m_panelManager = new PanelManager(mainWindow, m_widgetFactory);
        m_menuBuilder = new MenuBuilder(mainWindow);
        m_buttonCreator = new ButtonCreator(m_widgetFactory);
    }
    
    // Expose to JavaScript/YAML
    Q_INVOKABLE QWidget* createWidget(const QString &type, const QString &id) {
        return m_widgetFactory->createWidget(type, id);
    }
    
    Q_INVOKABLE QWidget* createPanel(const QString &id, const QString &title,
                                    const QString &position = "float") {
        return m_panelManager->createPanel(id, title, position);
    }
    
    Q_INVOKABLE QMenu* createMenu(const QString &id, const QString &title) {
        return m_menuBuilder->createMenu(id, title);
    }
    
    Q_INVOKABLE QPushButton* createButton(const QString &id, const QString &text,
                                         const QString &parentId = "") {
        QWidget *parent = nullptr;
        if (!parentId.isEmpty()) {
            parent = m_widgetFactory->getWidget(parentId);
        }
        return m_buttonCreator->createButton(id, text, parent);
    }
    
    Q_INVOKABLE void setProperty(const QString &widgetId, const QString &property,
                                 const QVariant &value) {
        if (QWidget *widget = m_widgetFactory->getWidget(widgetId)) {
            widget->setProperty(property.toUtf8(), value);
        }
    }
    
    Q_INVOKABLE void connectSignal(const QString &widgetId, const QString &signal,
                                  const QString &script) {
        // Connect Qt signal to script execution
        if (QWidget *widget = m_widgetFactory->getWidget(widgetId)) {
            // Use Qt's meta-object system to connect dynamically
            int signalIndex = widget->metaObject()->indexOfSignal(signal.toUtf8());
            if (signalIndex != -1) {
                // Store script and execute when signal fires
                // Implementation depends on script engine choice
            }
        }
    }
};
```

## YAML Configuration Schema

### Panel Definition
```yaml
panels:
  - id: search_panel
    title: "Search Results"
    position: bottom  # bottom, top, left, right, float
    width: 400
    height: 200
    layout: vertical  # vertical, horizontal, grid, form
    widgets:
      - type: QLineEdit
        id: search_input
        properties:
          placeholderText: "Enter search term..."
          maxLength: 100
      - type: QPushButton
        id: search_button
        text: "Search"
        icon: "system-search"
        connections:
          clicked: |
            var term = ui.getWidget('search_input').text;
            searchFiles(term);
      - type: QListView
        id: search_results
        properties:
          alternatingRowColors: true
```

### Menu Definition
```yaml
menus:
  - id: tools_menu
    title: "Tools"
    actions:
      - id: open_terminal
        text: "Open Terminal"
        shortcut: "Ctrl+T"
        icon: "utilities-terminal"
        script: |
          var terminal = ui.createPanel('terminal', 'Terminal', 'bottom');
          embedKonsole(terminal);
      - separator: true
      - id: custom_command
        text: "Custom Command..."
        shortcut: "Ctrl+Shift+C"
        script: |
          showCommandDialog();
      - submenu:
          id: advanced_menu
          title: "Advanced"
          actions:
            - id: reload_config
              text: "Reload Configuration"
              script: |
                reloadConfig();
```

### Toolbar Definition
```yaml
toolbars:
  - id: main_toolbar
    title: "Main Toolbar"
    position: top
    movable: true
    buttons:
      - id: refresh
        icon: "view-refresh"
        tooltip: "Refresh current directory"
        script: |
          activePanel.refresh();
      - id: split_view
        icon: "view-split-left-right"
        tooltip: "Split view"
        script: |
          splitPanels();
      - separator: true
      - id: terminal
        icon: "utilities-terminal"
        tooltip: "Open terminal here"
        script: |
          openTerminal(activePanel.currentDirectory());
```

### Custom Widget Creation
```yaml
custom_widgets:
  - id: file_preview
    type: composite
    layout: vertical
    widgets:
      - type: QLabel
        id: preview_title
        text: "File Preview"
        properties:
          alignment: "AlignCenter"
          font: "bold"
      - type: QTextEdit
        id: preview_content
        properties:
          readOnly: true
          lineWrapMode: "WidgetWidth"
      - type: QProgressBar
        id: loading_progress
        properties:
          visible: false
```

## JavaScript API

### Core UI Object
```javascript
ui = {
    // Widget creation
    createWidget: function(type, id, parent) {
        return UIScriptInterface.createWidget(type, id);
    },
    
    getWidget: function(id) {
        return UIScriptInterface.getWidget(id);
    },
    
    deleteWidget: function(id) {
        UIScriptInterface.deleteWidget(id);
    },
    
    // Panel management
    createPanel: function(id, title, position, width, height) {
        return UIScriptInterface.createPanel(id, title, position, width, height);
    },
    
    showPanel: function(id) {
        var panel = UIScriptInterface.getWidget(id);
        if (panel) panel.show();
    },
    
    hidePanel: function(id) {
        var panel = UIScriptInterface.getWidget(id);
        if (panel) panel.hide();
    },
    
    // Menu building
    createMenu: function(id, title, parent) {
        return UIScriptInterface.createMenu(id, title, parent);
    },
    
    addMenuItem: function(menuId, itemId, text, action) {
        var item = UIScriptInterface.addAction(menuId, itemId, text);
        if (item && action) {
            item.triggered.connect(action);
        }
        return item;
    },
    
    addSeparator: function(menuId) {
        UIScriptInterface.addSeparator(menuId);
    },
    
    showContextMenu: function(menuId, x, y) {
        UIScriptInterface.showContextMenu(menuId, x, y);
    },
    
    // Button creation
    createButton: function(config) {
        var btn = UIScriptInterface.createButton(
            config.id, 
            config.text,
            config.parent,
            config.icon,
            config.tooltip,
            config.style
        );
        
        if (btn && config.onClick) {
            btn.clicked.connect(config.onClick);
        }
        
        return btn;
    },
    
    // Layout management
    createLayout: function(type, parent) {
        if (type === 'vertical') return new QVBoxLayout(parent);
        if (type === 'horizontal') return new QHBoxLayout(parent);
        if (type === 'grid') return new QGridLayout(parent);
        if (type === 'form') return new QFormLayout(parent);
        return null;
    },
    
    addToLayout: function(layoutOrParent, widget, row, col) {
        if (layoutOrParent.addWidget) {
            if (row !== undefined && col !== undefined) {
                layoutOrParent.addWidget(widget, row, col);
            } else {
                layoutOrParent.addWidget(widget);
            }
        }
    },
    
    // Property manipulation
    setProperty: function(widgetId, property, value) {
        UIScriptInterface.setProperty(widgetId, property, value);
    },
    
    getProperty: function(widgetId, property) {
        var widget = UIScriptInterface.getWidget(widgetId);
        return widget ? widget[property] : undefined;
    },
    
    // Style manipulation
    setStyle: function(widgetId, styleSheet) {
        ui.setProperty(widgetId, 'styleSheet', styleSheet);
    },
    
    addClass: function(widgetId, className) {
        var widget = UIScriptInterface.getWidget(widgetId);
        if (widget) {
            widget.setProperty('class', className);
        }
    },
    
    // Signal connections
    connect: function(widgetId, signal, handler) {
        var widget = UIScriptInterface.getWidget(widgetId);
        if (widget && widget[signal]) {
            widget[signal].connect(handler);
        }
    },
    
    disconnect: function(widgetId, signal, handler) {
        var widget = UIScriptInterface.getWidget(widgetId);
        if (widget && widget[signal]) {
            widget[signal].disconnect(handler);
        }
    }
};
```

## Example Scripts

### Create Custom Search Panel
```javascript
function createSearchPanel() {
    // Create dock panel at bottom
    var panel = ui.createPanel('search_dock', 'File Search', 'bottom', 400, 200);
    
    // Create layout
    var layout = ui.createLayout('vertical', panel);
    
    // Create search input
    var searchBox = ui.createWidget('QLineEdit', 'search_box', panel);
    ui.setProperty('search_box', 'placeholderText', 'Search files...');
    ui.addToLayout(layout, searchBox);
    
    // Create button row
    var buttonLayout = ui.createLayout('horizontal');
    
    var searchBtn = ui.createButton({
        id: 'search_btn',
        text: 'Search',
        icon: 'system-search',
        onClick: function() {
            performSearch(searchBox.text);
        }
    });
    buttonLayout.addWidget(searchBtn);
    
    var clearBtn = ui.createButton({
        id: 'clear_btn',
        text: 'Clear',
        onClick: function() {
            searchBox.clear();
            clearResults();
        }
    });
    buttonLayout.addWidget(clearBtn);
    
    layout.addLayout(buttonLayout);
    
    // Create results list
    var results = ui.createWidget('QListView', 'search_results', panel);
    ui.setProperty('search_results', 'alternatingRowColors', true);
    ui.addToLayout(layout, results);
    
    // Connect double-click to open file
    ui.connect('search_results', 'doubleClicked', function(index) {
        openSearchResult(index);
    });
}
```

### Dynamic Toolbar Creation
```javascript
function createCustomToolbar(config) {
    var toolbar = ui.createWidget('QToolBar', config.id);
    mainWindow.addToolBar(toolbar);
    
    config.buttons.forEach(function(btnConfig) {
        if (btnConfig.separator) {
            toolbar.addSeparator();
        } else {
            var action = toolbar.addAction(btnConfig.icon, btnConfig.text);
            action.setToolTip(btnConfig.tooltip);
            action.triggered.connect(function() {
                executeCommand(btnConfig.command);
            });
        }
    });
    
    return toolbar;
}

// Usage
createCustomToolbar({
    id: 'file_ops_toolbar',
    buttons: [
        { icon: 'edit-copy', text: 'Copy', tooltip: 'Copy selected files', command: 'copy' },
        { icon: 'edit-cut', text: 'Cut', tooltip: 'Cut selected files', command: 'cut' },
        { icon: 'edit-paste', text: 'Paste', tooltip: 'Paste files', command: 'paste' },
        { separator: true },
        { icon: 'edit-delete', text: 'Delete', tooltip: 'Delete selected files', command: 'delete' }
    ]
});
```

### Context-Sensitive Menu
```javascript
function createFileContextMenu(fileItem) {
    var menuId = 'file_context_' + Date.now();
    var menu = ui.createMenu(menuId, 'File Actions');
    
    // Add common actions
    ui.addMenuItem(menuId, 'open', 'Open', function() {
        openFile(fileItem);
    });
    
    ui.addMenuItem(menuId, 'open_with', 'Open With...', function() {
        showOpenWithDialog(fileItem);
    });
    
    ui.addSeparator(menuId);
    
    // Add type-specific actions
    if (fileItem.isImage()) {
        ui.addMenuItem(menuId, 'preview', 'Preview', function() {
            showImagePreview(fileItem);
        });
        ui.addMenuItem(menuId, 'edit', 'Edit in GIMP', function() {
            openInEditor('gimp', fileItem);
        });
    } else if (fileItem.isText()) {
        ui.addMenuItem(menuId, 'edit', 'Edit in Kate', function() {
            openInEditor('kate', fileItem);
        });
        ui.addMenuItem(menuId, 'search_in', 'Search in File', function() {
            searchInFile(fileItem);
        });
    }
    
    ui.addSeparator(menuId);
    
    // Add file operations
    ui.addMenuItem(menuId, 'copy', 'Copy', function() {
        copyFile(fileItem);
    });
    ui.addMenuItem(menuId, 'cut', 'Cut', function() {
        cutFile(fileItem);
    });
    ui.addMenuItem(menuId, 'delete', 'Delete', function() {
        deleteFile(fileItem);
    });
    ui.addMenuItem(menuId, 'rename', 'Rename', function() {
        renameFile(fileItem);
    });
    
    return menu;
}
```

### Custom Dialog Creation
```javascript
function createCustomDialog(title, fields) {
    var dialog = ui.createWidget('QDialog', 'custom_dialog_' + Date.now());
    dialog.setWindowTitle(title);
    
    var layout = ui.createLayout('form', dialog);
    var inputs = {};
    
    fields.forEach(function(field) {
        var label = ui.createWidget('QLabel', field.id + '_label', dialog);
        label.setText(field.label);
        
        var input;
        switch (field.type) {
            case 'text':
                input = ui.createWidget('QLineEdit', field.id, dialog);
                if (field.placeholder) {
                    ui.setProperty(field.id, 'placeholderText', field.placeholder);
                }
                break;
            case 'number':
                input = ui.createWidget('QSpinBox', field.id, dialog);
                if (field.min) ui.setProperty(field.id, 'minimum', field.min);
                if (field.max) ui.setProperty(field.id, 'maximum', field.max);
                break;
            case 'checkbox':
                input = ui.createWidget('QCheckBox', field.id, dialog);
                input.setText(field.text || '');
                break;
            case 'combo':
                input = ui.createWidget('QComboBox', field.id, dialog);
                if (field.options) {
                    field.options.forEach(function(opt) {
                        input.addItem(opt);
                    });
                }
                break;
        }
        
        inputs[field.id] = input;
        layout.addRow(label, input);
    });
    
    // Add buttons
    var buttonBox = ui.createWidget('QDialogButtonBox', 'button_box', dialog);
    buttonBox.setStandardButtons(QDialogButtonBox.Ok | QDialogButtonBox.Cancel);
    
    ui.connect('button_box', 'accepted', function() {
        var values = {};
        Object.keys(inputs).forEach(function(key) {
            values[key] = inputs[key].value || inputs[key].text;
        });
        dialog.accept();
        return values;
    });
    
    ui.connect('button_box', 'rejected', function() {
        dialog.reject();
    });
    
    layout.addRow(buttonBox);
    
    return dialog;
}

// Usage
var dialog = createCustomDialog('Search Options', [
    { id: 'pattern', type: 'text', label: 'Search Pattern:', placeholder: '*.txt' },
    { id: 'case_sensitive', type: 'checkbox', label: 'Case Sensitive:', text: 'Match case' },
    { id: 'search_type', type: 'combo', label: 'Search Type:', options: ['Name', 'Content', 'Both'] },
    { id: 'max_results', type: 'number', label: 'Max Results:', min: 1, max: 1000 }
]);
```

## Integration with CommandMaster

### Initialization
```c++
// In CommandMaster main window constructor
class CommandMaster : public QMainWindow {
private:
    UIScriptInterface *m_uiInterface;
    QJSEngine *m_scriptEngine;
    
public:
    CommandMaster() {
        // Create UI interface
        m_uiInterface = new UIScriptInterface(this);
        
        // Create script engine
        m_scriptEngine = new QJSEngine(this);
        
        // Expose UI interface to scripts
        QJSValue uiObj = m_scriptEngine->newQObject(m_uiInterface);
        m_scriptEngine->globalObject().setProperty("UIScriptInterface", uiObj);
        
        // Load JavaScript UI API
        loadScriptFile(":/scripts/ui-api.js");
        
        // Load user scripts
        loadUserScripts();
    }
    
    void loadScriptFile(const QString &path) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QString script = file.readAll();
            m_scriptEngine->evaluate(script);
        }
    }
};
```

### YAML Loading
```c++
void loadUIFromYAML(const QString &yamlPath) {
    YAML::Node config = YAML::LoadFile(yamlPath.toStdString());
    
    // Load panels
    if (config["panels"]) {
        for (const auto& panel : config["panels"]) {
            QString id = QString::fromStdString(panel["id"].as<std::string>());
            QString title = QString::fromStdString(panel["title"].as<std::string>());
            QString position = QString::fromStdString(panel["position"].as<std::string>());
            
            QWidget *panelWidget = m_uiInterface->createPanel(id, title, position);
            
            // Create widgets in panel
            if (panel["widgets"]) {
                for (const auto& widget : panel["widgets"]) {
                    QString type = QString::fromStdString(widget["type"].as<std::string>());
                    QString widgetId = QString::fromStdString(widget["id"].as<std::string>());
                    
                    m_uiInterface->createWidget(type, widgetId);
                    
                    // Set properties
                    if (widget["properties"]) {
                        for (const auto& prop : widget["properties"]) {
                            QString propName = QString::fromStdString(prop.first.as<std::string>());
                            QVariant propValue = /* convert YAML value to QVariant */;
                            m_uiInterface->setProperty(widgetId, propName, propValue);
                        }
                    }
                }
            }
        }
    }
    
    // Load menus
    if (config["menus"]) {
        // Similar pattern for menus
    }
    
    // Load toolbars
    if (config["toolbars"]) {
        // Similar pattern for toolbars
    }
}
```

## Security Considerations

### Script Sandboxing
```c++
class ScriptSandbox {
private:
    QStringList m_allowedWidgetTypes;
    QStringList m_allowedProperties;
    int m_maxWidgets = 100;
    int m_currentWidgetCount = 0;
    
public:
    bool canCreateWidget(const QString &type) {
        return m_allowedWidgetTypes.contains(type) && 
               m_currentWidgetCount < m_maxWidgets;
    }
    
    bool canSetProperty(const QString &property) {
        // Block dangerous properties
        QStringList dangerous = {"dynamicPropertyNames", "metaObject"};
        return !dangerous.contains(property) && 
               m_allowedProperties.contains(property);
    }
};
```

### Resource Limits
```c++
class ResourceLimiter {
public:
    void enforceMemoryLimit(size_t maxBytes) {
        // Monitor memory usage
    }
    
    void enforceTimeLimit(int maxMilliseconds) {
        // Use QTimer to kill long-running scripts
    }
    
    void enforceWidgetLimit(int maxWidgets) {
        // Track widget creation
    }
};
```

## Performance Optimization

### Lazy Loading
```c++
class LazyWidgetLoader {
public:
    void registerWidgetFactory(const QString &type, 
                              std::function<QWidget*(QWidget*)> factory) {
        m_factories[type] = factory;
    }
    
    QWidget* createWidget(const QString &type, QWidget *parent) {
        if (m_factories.contains(type)) {
            return m_factories[type](parent);
        }
        return nullptr;
    }
    
private:
    QMap<QString, std::function<QWidget*(QWidget*)>> m_factories;
};
```

### Widget Pooling
```c++
class WidgetPool {
private:
    QMap<QString, QQueue<QWidget*>> m_pool;
    
public:
    QWidget* acquire(const QString &type) {
        if (m_pool[type].isEmpty()) {
            return createNewWidget(type);
        }
        return m_pool[type].dequeue();
    }
    
    void release(QWidget *widget) {
        widget->hide();
        widget->setParent(nullptr);
        QString type = widget->metaObject()->className();
        m_pool[type].enqueue(widget);
    }
};
```

## Future Enhancements

### 1. Visual Designer Integration
- Drag-and-drop UI builder
- Property inspector
- WYSIWYG editing
- Generate YAML from visual design

### 2. Advanced Scripting Features
- Script debugging
- Performance profiling
- Script marketplace
- Version control integration

### 3. Theme System
- Complete theme engine
- Import themes from other applications
- Live theme preview
- Per-widget styling

### 4. Plugin Architecture
- C++ plugin support
- Python bindings
- Web-based UI components
- Remote UI rendering

## Summary

The UI-Complex system provides CommandMaster with unprecedented flexibility in UI customization. Scripts can:

1. Create any Qt widget dynamically
2. Build complex layouts programmatically
3. Generate menus and toolbars on demand
4. Respond to user actions with custom UI
5. Integrate seamlessly with existing CommandMaster functionality

This system forms the foundation for CommandMaster's evolution from a simple file manager to a fully scriptable, user-customizable command center.
