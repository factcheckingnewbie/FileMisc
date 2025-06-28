# Full Connection Diagram and Extension Guide

---

## 1. Top-Down Connection Diagram

```
main.cpp
│
├─ main()
│   ├─ CmdMasterParser::parseFile("CommandMasterApp.dsl", config)
│   │     └─ fills CmdMasterConfig (menus, buttons, splitters, commands)
│   ├─ Sets up QMainWindow (title, size)
│   ├─ QMenuBar (for config.menuBarPanels)
│   │     └─ for each menu panel:
│   │           └─ for each menu item:
│   │                 └─ addMenuItems(menu, menuDef.subItems, parent, config)
│   │                        └─ recursively builds menus/submenus,
│   │                           creates QAction, connects lambda (accesses config)
│   ├─ QToolBar(s) (for config.buttonBarPanels)
│   │     └─ for each button:
│   │           └─ creates QAction, connects lambda (can access config)
│   ├─ QSplitter (for config.splitterPanels)
│   │     └─ sets up placeholder widgets, applies sizes
│   └─ mainWindow.show(), app.exec()
│
├─ addMenuItems(menu, items, parent, config)
│   └─ for each CmdMasterMenuItem:
│         ├─ creates QAction (if not separator)
│         ├─ connects lambda (can lookup command in config)
│         └─ recursively calls addMenuItems for submenus
│
└─ (QAction::triggered lambda)
      └─ uses item.action/btn.action to lookup CmdMasterCommand in config.commands
           └─ (future: execute command, show dialog, etc)

cmdmaster_parser.cpp/.h
├─ CmdMasterParser::parseFile()
│   └─ parses DSL, fills CmdMasterConfig (panels, commands, etc)

CmdMasterConfig & related structs
├─ CmdMasterCommand (name, alias, description, shell, etc)
├─ CmdMasterMenuItem (label, action, shortcut, icon, subItems)
├─ CmdMasterButton (label, action, icon)
├─ CmdMasterSplitter (orientation, sizes)
├─ CmdMasterPanel (type, menuItems/buttons/splitter, attributes)
└─ CmdMasterConfig (menuBarPanels, buttonBarPanels, splitterPanels, commands)

CommandMasterApp.dsl
└─ User-editable configuration: menus, buttons, splitters, commands

```

---

## 2. Function and Data Structure Descriptions

### main.cpp

- **main()**
  - Initializes Qt application, parses config, sets up main window.
  - Builds menubar, toolbars, splitters from config.
  - Shows main window and enters event loop.

- **addMenuItems(QMenu*, QList<CmdMasterMenuItem>, QWidget*, const CmdMasterConfig&)**
  - Recursively creates menu actions/submenus.
  - Connects each QAction to a lambda that can lookup commands in config.

- **QAction::triggered lambda**
  - For menu/toolbar actions, looks up command details in config.
  - Can be extended to execute commands, show dialogs, etc.

### cmdmaster_parser.cpp/.h

- **CmdMasterParser::parseFile()**
  - Reads DSL, parses all panel and command definitions, fills CmdMasterConfig.

- **Data Structures**
  - Hold all configuration for building the UI and mapping actions to commands.

### CommandMasterApp.dsl

- User-editable file for all UI (menus, buttons, splitters) and commands.

---

## 3. How to Add/Extend Methods or UI Elements

### **A. For New Menu Actions or Commands**

1. **DSL:**  
   Add new menu items or commands in CommandMasterApp.dsl.

2. **Parser:**  
   Ensure parser supports new fields or formats (update if needed).

3. **Config:**  
   Verify new items are present in CmdMasterConfig.

4. **UI Logic:**  
   No code changes needed for simple menu/button additions.
   For new action types or behaviors, update lambda in addMenuItems or toolbar logic.

5. **Handler:**  
   Extend lambda(s) to support new command types or actions as required.

### **B. For New UI Components (e.g., sidebars, status bars)**

1. Add new Panel sections to DSL.
2. Extend parser/config structs.
3. In main.cpp, add code to construct new UI components from config.
4. Connect actions as needed.

### **C. For New Action Types (e.g., context menus, shortcuts)**

1. Extend DSL and parser.
2. Add new helper functions (e.g., addContextMenuItems).
3. Call new helpers from main.cpp, passing config.
4. Update diagrams to show new connections.

---

## 4. Step-by-Step Example: Adding a New Command Action

### **Step 1: Add to DSL**
```dsl
FoldableMenu "Tools"
    Item "New Action" action:new_action shortcut:Ctrl+N icon:new-icon

Command new_action
    Description: "Does something new"
    Args:
    Out:
    Shell: echo "New action triggered"
```

### **Step 2: Ensure Parsing Support**
- Update parser and data structures if new fields are needed.

### **Step 3: UI and Execution**
- Menu item will be created and connected automatically if DSL and parser are correct.
- Lambda in addMenuItems can be extended for new behaviors.

### **Step 4: Extending the Lambda**

```cpp
QObject::connect(action, &QAction::triggered, [item, &config]() {
    auto cmd = config.commands.value(item.action);
    if (!cmd.name.isEmpty()) {
        if (cmd.shell.startsWith("python:")) {
            // Execute Python script, etc.
        } else {
            // Default shell execution
        }
    }
});
```
---

## 5. Discussion and Best Practices

- **All new features should be represented in the DSL, parsed into config, and then connected in main.cpp.**
- **Lambdas should be extended for new command types/behaviors, but the overall architecture stays scalable and maintainable.**
- **Always update diagrams and documentation to reflect new flows and relationships.**

---

## 6. Visual Summary

```
DSL (CommandMasterApp.dsl)
    ↓
Parser (cmdmaster_parser.cpp)
    ↓
Config Structs (CmdMasterConfig, etc)
    ↓
main.cpp (UI/build/connect)
    ↓
Lambdas (lookup in config, execute method)
```

---

### **Change Steps for Any Extension:**

1. **DSL:** Add/extend description.
2. **Parser:** Update if syntax/structure changes.
3. **Config:** Add/modify struct as needed.
4. **main.cpp:** Update loops, helpers, lambdas.
5. **Handler/Logic:** Implement new functions as needed.
6. **Diagram:** Update call/data flow diagrams.

---

*If you need a graphical diagram, or a detailed example for a specific new method, just ask!*