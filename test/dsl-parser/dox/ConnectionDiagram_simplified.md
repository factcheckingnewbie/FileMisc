# Code Overview (Brief)

## Main Flow

```
main.cpp
  |
  +-> CmdMasterParser::parseFile()   // parses DSL, fills config
  +-> QMainWindow setup
  +-> Build QMenuBar from config.menuBarPanels (calls addMenuItems recursively)
  +-> Build QToolBar(s) from config.buttonBarPanels
  +-> Build QSplitter from config.splitterPanels
  +-> mainWindow.show(), app.exec()
```

## Data Structures

- **config**: Holds menus, buttons, splitters, commands.
- **addMenuItems**: Recursively builds menus/submenus, connects actions to command lookups.

## Extension Steps

1. Add new items/commands in DSL.
2. Ensure parser/config support.
3. Update/extend UI logic in main.cpp as needed.
4. Connect new actions to handlers (lambda).

## Visual Summary

```
DSL → Parser → Config Structs → main.cpp (UI/build/connect) → Lambda Handlers
```