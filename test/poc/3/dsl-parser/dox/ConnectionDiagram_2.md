# Connection Diagram and Extension Guide for `cmdmaster` Main Application

---

## Table of Contents

1. [Full Connection Diagram](#full-connection-diagram)
2. [Function/Component Descriptions](#functioncomponent-descriptions)
3. [How to Extend: Adding More Methods](#how-to-extend-adding-more-methods)
4. [Step-by-Step Example: Adding a new Command Action](#step-by-step-example-adding-a-new-command-action)
5. [Discussion: Why These Changes Are Needed](#discussion-why-these-changes-are-needed)

---

## Full Connection Diagram

Below is a full (text-based) connection diagram, showing the principal files, classes, functions, data structures, and their relationships in the current codebase.  
The diagram is written top-down, with arrows (`→`) indicating the direction of calls or data flow.

```
main.cpp
│
├─ main()
│   │
│   ├─ calls CmdMasterParser::parseFile()
│   │     │
│   │     └─ fills CmdMasterConfig (contains menuBarPanels, buttonBarPanels, splitterPanels, commands, etc)
│   │
│   ├─ sets up QMainWindow
│   │
│   ├─ constructs QMenuBar from config.menuBarPanels
│   │     │
│   │     └─ for each menu panel
│   │           │
│   │           └─ for each menu item
│   │                 │
│   │                 └─ calls addMenuItems(menu, items, parent, config)
│   │                          │
│   │                          └─ recursively builds menus and submenus
│   │                               │
│   │                               └─ for each QAction, connects lambda to item and config
│   │
│   ├─ constructs QToolBar(s) from config.buttonBarPanels
│   │     │
│   │     └─ for each button, creates QAction and connects lambda (can also use config for command lookup)
│   │
│   ├─ constructs QSplitter from config.splitterPanels (if present)
│   │     │
│   │     └─ sets up placeholder widgets (can be extended)
│   │
│   └─ shows mainWindow
│
├─ addMenuItems(menu, items, parent, config)
│   │
│   ├─ for each CmdMasterMenuItem
│   │     ├─ creates QAction (if not separator)
│   │     ├─ connects lambda to item and config (for command lookup)
│   │     └─ recursively calls addMenuItems for submenus
│
└─ (lambda in QAction::triggered) [in both menubar and buttonbar]
     │
     └─ uses item.action or btn.action to lookup CmdMasterCommand in config.commands
          └─ (future: execute command, show dialog, etc)

cmdmaster_parser.cpp
│
├─ CmdMasterParser::parseFile()
│   │
│   ├─ Loads and parses DSL file (CommandMasterApp.dsl)
│   └─ Builds CmdMasterConfig, fills all relevant fields according to DSL structure

cmdmaster_parser.h
│
├─ Defines data structures:
│   ├─ CmdMasterCommand
│   ├─ CmdMasterMenuItem
│   ├─ CmdMasterButton
│   ├─ CmdMasterSplitter
│   ├─ CmdMasterPanel
│   └─ CmdMasterConfig

CommandMasterApp.dsl
│
└─ User-editable configuration file (menus, buttons, splitters, commands, etc)
```

---

## Function/Component Descriptions

### main.cpp

- **main()**
  - Entry point. Sets up environment, parses configuration, builds main window, populates menubar, buttonbar, splitter, and starts application event loop.

- **addMenuItems(QMenu*, QList<CmdMasterMenuItem>, QWidget*, const CmdMasterConfig&)**
  - Recursive helper to create menus and submenus from parsed config.
  - For each menu item (recursively):
    - Creates an action or separator.
    - Connects a lambda which can access both the menu item and the parsed config (so it can look up and trigger commands).

- **QAction::triggered lambda**
  - For menu actions and toolbar buttons, this lambda is connected to the triggered signal.
  - Inside, it looks up the command by action name and can (future: will) execute the command or show its description, etc.

### cmdmaster_parser.cpp / .h

- **CmdMasterParser::parseFile()**
  - Parses the DSL file and fills a CmdMasterConfig struct.
  - Handles all panel types, commands, etc.

- **CmdMasterConfig and related structs**
  - Data structures to hold all parsed configuration for the UI and commands.

### CommandMasterApp.dsl

- User-editable file describing menus, buttons, splitters, and commands.

---

## How to Extend: Adding More Methods

Suppose you want to add more **methods**—for example, new actions in the menu or new command execution types.  
Here’s what you need to know and do:

### 1. **Where to Add Data**

- **CommandMasterApp.dsl**:  
  - Add new menu items, buttons, or command definitions in the DSL.
- **cmdmaster_parser.cpp / .h**:  
  - Make sure new definitions or fields in DSL are parsed and stored in the relevant structs.
  - If you want to support new menu item types or button types, update the parser and data structures accordingly.

### 2. **Where to Add Functionality**

- **main.cpp**:
  - If you want to support new action types, or new command execution logic, you need to update the lambda connected to QAction::triggered.
  - You may need to pass extra context or arguments (for example, arguments to commands) from config or UI.

- **addMenuItems**:
  - If your new method requires new menu item types (like checkable, radio, etc.), add handling for those in addMenuItems.

- **ButtonBar**:
  - Add new buttons to the DSL and parser, and make sure they are constructed in the main window.

### 3. **How to Connect New Methods**

- **Connection Flow**:
  - New menu items or buttons defined in the DSL are parsed into config.
  - When building the UI, addMenuItems or the buttonbar logic creates QActions.
  - The lambda connected to triggered can use the action (item.action or btn.action) to look up the new method/command in config.
  - You can extend the lambda to handle new actions or command types as needed.

---

## Step-by-Step Example: Adding a New Command Action

Suppose you want to add a new menu item which performs a new type of operation.

### Step 1: Add to DSL
```dsl
FoldableMenu "Tools"
    Item "New Action" action:new_action shortcut:Ctrl+N icon:new-icon
```
And add a command:
```dsl
Command new_action
    Description: "Does something new"
    Args:
    Out:
    Shell: echo "New action triggered"
```

### Step 2: Ensure Parsing Support
- If your new action needs special parsing (new fields, new syntax), update `cmdmaster_parser.cpp` and the relevant data structures in `cmdmaster_parser.h`.

### Step 3: UI and Execution
- The action appears in the menu because it is parsed into `config`.
- `addMenuItems` creates the QAction and connects the lambda.
- The lambda, when triggered, looks up `config.commands.value("new_action")` and can execute its logic.

### Step 4: Extending the Lambda
- If you want to support new behaviors (e.g., not just shell, but Python scripts, or plugins), extend the lambda.
- Example:
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

## Discussion: Why These Changes Are Needed

- **Every connection in the diagram represents a flow of data or control.**
- **When you add more methods (e.g., new command actions, new menu item types), you must ensure that:**
    - The _user input_ (typically the DSL) can express the new method.
    - The _parser and config structures_ in code can parse and hold the new method.
    - The _UI builder code_ (main.cpp/addMenuItems/etc) can generate the appropriate UI elements.
    - The _event handling code_ (lambda connected to QAction::triggered) knows how to look up and execute the new method.

- **If you do not update all these places, your new method may not appear, may not be callable, or may not have a handler to execute its logic.**

- **The code is designed so that methods are described once (in the DSL), parsed into a central config, and then connected at runtime via lookup in the config and lambda handlers.**
- **To scale the system for more methods, always keep the flow in mind: DSL → parser → config → UI → lambda handler → execution.**

```

---
---
---

### C. **Summary of change steps (for any extension):**

1. **DSL:** Extend the description file.
2. **Parser:** Update parser logic if the syntax/structure changes.
3. **Config:** Add/modify config structure as needed.
4. **main.cpp:** Add or update loops, helpers, and lambda handlers.
   - Pass `config` as needed for lookups.
   - Add dispatch to new handler methods if required.
5. **Handler/Logic:** Write new methods for new actions/UI.
6. **Diagram:** Add new function/object arrows for new relationships.

```
-
**This diagram and guide should equip you to confidently extend the codebase for new methods and understand exactly where changes are needed in the data flow and function connections. If you need a graphical diagram, or want to see a worked example for a specific new method, just ask.**