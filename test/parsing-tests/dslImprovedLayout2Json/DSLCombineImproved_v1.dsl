Window "Shell Commander" width:1024 height:768

Panel menubar position:top color:green
    FoldableMenu "File"
        Item "Open Script" action:open_script shortcut:Ctrl+O icon:file-open
        Item "Save Script" action:save_script shortcut:Ctrl+S icon:file-save
        Item "Save As" action:save_script_as shortcut:Ctrl+Shift+S icon:file-save-as
        Separator
        Item "Exit" action:exit_app shortcut:Ctrl+Q icon:app-exit
    FoldableMenu "Edit"
        Item "Undo" action:undo shortcut:Ctrl+Z icon:edit-undo
        Item "Redo" action:redo shortcut:Ctrl+Y icon:edit-redo
        Separator
        Item "Cut" action:cut shortcut:Ctrl+X icon:edit-cut
        Item "Copy" action:copy shortcut:Ctrl+C icon:edit-copy
        Item "Paste" action:paste shortcut:Ctrl+V icon:edit-paste
    FoldableMenu "Commands"
        Submenu "Shell"
            Item "Execute Command" action:show_exec_dialog shortcut:Ctrl+E icon:terminal
            Item "Set Environment" action:show_env_dialog icon:settings
        Submenu "Files"
            Item "List Files" action:show_ls_dialog shortcut:Ctrl+L icon:folder
            Item "Find Files" action:show_find_dialog shortcut:Ctrl+F icon:search
            Item "Search Pattern" action:show_grep_dialog shortcut:Ctrl+G icon:filter
            Item "Transform (sed)" action:show_sed_dialog icon:transform
            Item "Process (awk)" action:show_awk_dialog icon:code
        Submenu "System"
            Item "Process List" action:show_ps_dialog shortcut:Ctrl+P icon:list
            Item "Kill Process" action:show_kill_dialog icon:stop
            Item "SSH Command" action:show_ssh_dialog icon:network
            Item "HTTP Request" action:show_curl_dialog icon:download
    FoldableMenu "Flows"
        Item "Clean Logs" action:run_clean_logs icon:broom
        Item "Remote Backup" action:show_backup_dialog icon:backup
        Item "API Monitor" action:show_monitor_dialog icon:monitor
        Separator
        Item "Flow Editor" action:show_flow_editor shortcut:Ctrl+Shift+F icon:flow
    FoldableMenu "View"
        Item "Show Output" action:toggle_output checked:true
        Item "Show History" action:toggle_history checked:true
        Item "Show Variables" action:toggle_variables checked:false
    FoldableMenu "Help"
        Item "Command Reference" action:show_commands
        Item "About" action:show_about

Panel toolbar position:top color:lightgray
    Button "Execute" action:quick_exec icon:play
    Button "List Files" action:show_ls_dialog icon:folder
    Button "Find" action:show_find_dialog icon:search
    Button "SSH" action:show_ssh_dialog icon:network
    Separator
    Button "Stop" action:stop_current icon:stop enabled:false
    Button "Clear" action:clear_output icon:clear

Panel splitter orientation:horizontal sizes:250,774
    Panel sidebar color:darkgray
        TabView
            Tab "History"
                List history_list action:replay_command
            Tab "Variables"
                Tree variable_tree
            Tab "Flows"
                List flow_list action:run_flow
    
    Panel splitter orientation:vertical sizes:500,268
        Panel editor
            TabView
                Tab "Command"
                    TextArea command_input syntax:shell
                Tab "Script"
                    TextArea script_editor syntax:dsl
                Tab "Output"
                    TextArea output_view readonly:true monospace:true
        
        Panel bottom_tabs
            TabView
                Tab "Console"
                    TextArea console_output readonly:true monospace:true color:black bgcolor:darkblue
                Tab "Errors"
                    TextArea error_output readonly:true color:red
                Tab "Debug"
                    Table debug_table columns:"Time,Command,Status,Duration"

# Dialog definitions for commands

Dialog "Execute Command" id:exec_dialog width:500 height:200
    Label "Command:" position:10,10
    TextArea cmd_input position:10,35 width:480 height:100 monospace:true
    Button "Execute" position:330,160 action:exec_command default:true
    Button "Cancel" position:410,160 action:close_dialog

Dialog "List Files" id:ls_dialog width:400 height:150
    Label "Directory:" position:10,10
    TextField dir_input position:10,35 width:380 placeholder:"Leave empty for current"
    Checkbox "Show hidden" id:show_hidden position:10,70
    Checkbox "Long format" id:long_format position:150,70 checked:true
    Button "List" position:230,110 action:exec_ls default:true
    Button "Cancel" position:310,110 action:close_dialog

Dialog "Find Files" id:find_dialog width:450 height:200
    Label "Search Path:" position:10,10
    TextField path_input position:10,35 width:430 value:"."
    Label "File Pattern:" position:10,70
    TextField pattern_input position:10,95 width:430 placeholder:"*.log"
    Checkbox "Case sensitive" id:case_sensitive position:10,130
    Button "Find" position:280,160 action:exec_find default:true
    Button "Cancel" position:360,160 action:close_dialog

Dialog "Search Pattern" id:grep_dialog width:500 height:250
    Label "Pattern (regex):" position:10,10
    TextField grep_pattern position:10,35 width:480
    Label "Input Source:" position:10,70
    RadioGroup input_source position:10,95
        Radio "From variable" value:variable checked:true
        Radio "From file" value:file
        Radio "From last output" value:pipe
    ComboBox var_select position:150,95 width:200 items:$variables visible:true
    TextField file_select position:150,95 width:200 visible:false
    Checkbox "Case insensitive" id:case_insensitive position:10,130
    Checkbox "Invert match" id:invert_match position:150,130
    Button "Search" position:340,210 action:exec_grep default:true
    Button "Cancel" position:420,210 action:close_dialog

Dialog "SSH Command" id:ssh_dialog width:500 height:300
    Label "Host:" position:10,10
    TextField host_input position:10,35 width:480 placeholder:"user@hostname"
    Label "Command:" position:10,70
    TextArea ssh_cmd position:10,95 width:480 height:100
    Checkbox "Use key" id:use_key position:10,205 checked:true
    TextField key_path position:100,205 width:300 placeholder:"~/.ssh/id_rsa"
    Button "Connect" position:340,260 action:exec_ssh default:true
    Button "Cancel" position:420,260 action:close_dialog

Dialog "Remote Backup" id:backup_dialog width:550 height:350
    Label "SSH Host:" position:10,10
    TextField backup_host position:10,35 width:530
    Label "Source Path:" position:10,70
    TextField backup_src position:10,95 width:530 placeholder:"/path/to/source"
    Label "Destination:" position:10,130
    TextField backup_dst position:10,155 width:530 placeholder:"/backup/location"
    GroupBox "Options" position:10,195 width:530 height:100
        Checkbox "Compress" id:compress position:20,215 checked:true
        Checkbox "Incremental" id:incremental position:150,215
        Checkbox "Delete removed" id:delete_removed position:280,215
        Label "Exclude:" position:20,245
        TextField exclude_pattern position:80,245 width:200 placeholder:"*.tmp,*.log"
    Button "Start Backup" position:380,310 action:exec_backup default:true
    Button "Cancel" position:470,310 action:close_dialog

Dialog "API Monitor" id:monitor_dialog width:500 height:400
    Label "API URL:" position:10,10
    TextField api_url position:10,35 width:480
    Label "Search Pattern:" position:10,70
    TextField api_pattern position:10,95 width:480
    Label "Check Interval (seconds):" position:10,130
    SpinBox interval position:10,155 min:10 max:3600 value:60
    GroupBox "Actions" position:10,195 width:480 height:120
        Checkbox "Desktop notification" id:notify position:20,215 checked:true
        Checkbox "Log to file" id:log_file position:20,240
        TextField log_path position:120,240 width:250 placeholder:"/tmp/monitor.log"
        Checkbox "Execute command" id:exec_on_match position:20,265
        TextField match_cmd position:120,265 width:250
    Button "Start Monitor" position:330,360 action:start_monitor default:true
    Button "Cancel" position:420,360 action:close_dialog

# Status bar at bottom
Panel statusbar position:bottom height:25 color:gray
    Label "Ready" id:status_text position:5,3
    Separator position:200,0
    Label "Variables: 0" id:var_count position:210,3
    Separator position:300,0
    Label "History: 0" id:history_count position:310,3
    ProgressBar id:progress position:right width:150 visible:false
