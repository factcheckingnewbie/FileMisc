Window "File Manager" width:800 height:600

Panel menubar position:top color:green
    FoldableMenu "File"
        Item "Open File" action:open_file shortcut:Ctrl+O icon:file-open
        Item "Save" action:save_file shortcut:Ctrl+S icon:file-save
        Item "Save As" action:save_as_file shortcut:Ctrl+Shift+S icon:file-save-as
        Item "Close" action:close_file shortcut:Ctrl+W icon:file-close
        Separator
        Item "Exit" action:exit_app shortcut:Ctrl+Q icon:app-exit
    FoldableMenu "Edit"
        Item "Undo" action:undo shortcut:Ctrl+Z icon:edit-undo
        Item "Redo" action:redo shortcut:Ctrl+Y icon:edit-redo
        Separator
        Item "Cut" action:cut shortcut:Ctrl+X icon:edit-cut
        Item "Copy" action:copy shortcut:Ctrl+C icon:edit-copy
        Item "Paste" action:paste shortcut:Ctrl+V icon:edit-paste
    FoldableMenu "View"
        Item "Show Sidebar" action:toggle_sidebar checked:true
        Item "Show Preview" action:toggle_preview checked:true
    FoldableMenu "Tools"
        Item "List Files" action:list_files shortcut:Ctrl+L icon:list
        Item "Find Pattern" action:find_pattern shortcut:Ctrl+F icon:search
        Item "Count Matches" action:count_matches shortcut:Ctrl+M icon:numbers
    FoldableMenu "Help"
        Item "About" action:show_about
        Item "Help" action:show_help shortcut:F1

Panel splitter orientation:horizontal sizes:300,500

Panel buttonbar position:top color:lightgray
    Button "Open" action:open_file icon:file-open
    Button "Save" action:save_file icon:file-save
    Button "Copy" action:copy icon:edit-copy
    Button "Paste" action:paste icon:edit-paste
    Button "About" action:show_about icon:help

Commands
    Group files label:"File Operations"
        Command list_files alias:ls description:"List files in a directory" args:directory out:filelist shell:'ls "{directory}"'
        Command find_pattern alias:grep description:"Find a pattern in a file list" args:pattern,in:filelist out:matches shell:'echo "{in}" | grep "{pattern}"'
        Command count_matches alias:count description:"Count number of matches" args:in:matches out:count shell:'echo "{in}" | wc -l'
    Pipeline "Count files matching foo"
        Step list_files directory:"/tmp"
        Step find_pattern pattern:"foo" in:@list_files
        Step count_matches in:@find_pattern
