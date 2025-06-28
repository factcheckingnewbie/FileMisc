Window "Shell Commander" height:768 width:1024
Panel position:top color:green menubar:true
FoldableMenu "File"
Item "Open Script" action:open_script icon:file-open shortcut:Ctrl+O
Item "Save Script" action:save_script icon:file-save shortcut:Ctrl+S
Item "Save As" action:save_script_as icon:file-save-as shortcut:Ctrl+Shift+S
Separator
Item "Exit" action:exit_app icon:app-exit shortcut:Ctrl+Q
FoldableMenu "Edit"
Item "Undo" action:undo icon:edit-undo shortcut:Ctrl+Z
Item "Redo" action:redo icon:edit-redo shortcut:Ctrl+Y
Separator
Item "Cut" action:cut icon:edit-cut shortcut:Ctrl+X
Item "Copy" action:copy icon:edit-copy shortcut:Ctrl+C
Item "Paste" action:paste icon:edit-paste shortcut:Ctrl+V
FoldableMenu "Commands"
Submenu "Shell"
Item "Execute Command" action:show_exec_dialog icon:terminal shortcut:Ctrl+E
Item "Set Environment" action:show_env_dialog icon:settings
Submenu "Files"
Item "List Files" action:show_ls_dialog icon:folder shortcut:Ctrl+L
Item "Find Files" action:show_find_dialog icon:search shortcut:Ctrl+F
Item "Search Pattern" action:show_grep_dialog icon:filter shortcut:Ctrl+G
Item "Transform (sed)" action:show_sed_dialog icon:transform
Item "Process (awk)" action:show_awk_dialog icon:code
Submenu "System"
Item "Process List" action:show_ps_dialog icon:list shortcut:Ctrl+P
Item "Kill Process" action:show_kill_dialog icon:stop
Item "SSH Command" action:show_ssh_dialog icon:network
Item "HTTP Request" action:show_curl_dialog icon:download
FoldableMenu "Flows"
Item "Clean Logs" action:run_clean_logs icon:broom
Item "Remote Backup" action:show_backup_dialog icon:backup
Item "API Monitor" action:show_monitor_dialog icon:monitor
Separator
Item "Flow Editor" action:show_flow_editor icon:flow shortcut:Ctrl+Shift+F
FoldableMenu "View"
Item "Show Output" action:toggle_output checked:true
Item "Show History" action:toggle_history checked:true
Item "Show Variables" action:toggle_variables checked:false
FoldableMenu "Help"
Item "Command Reference" action:show_commands
Item "About" action:show_about
Panel position:top color:lightgray toolbar:true
Button "Execute" action:quick_exec icon:play
Button "List Files" action:show_ls_dialog icon:folder
Button "Find" action:show_find_dialog icon:search
Button "SSH" action:show_ssh_dialog icon:network
Separator
Button "Stop" action:stop_current enabled:false icon:stop
Button "Clear" action:clear_output icon:clear
Panel orientation:horizontal sizes:250 splitter:true
Panel color:darkgray sidebar:true
TabView
Tab "History"
List action:replay_command history_list:true
Tab "Variables"
Tree variable_tree:true
Tab "Flows"
List action:run_flow flow_list:true
Panel orientation:vertical sizes:500 splitter:true
Panel editor:true
TabView
Tab "Command"
TextArea command_input:true syntax:shell
Tab "Script"
TextArea script_editor:true syntax:dsl
Tab "Output"
TextArea monospace:true output_view:true readonly:true
Panel bottom_tabs:true
TabView
Tab "Console"
TextArea bgcolor:darkblue color:black console_output:true monospace:true readonly:true
Tab "Errors"
TextArea color:red error_output:true readonly:true
Tab "Debug"
Table columns:"Time,Command,Status,Duration" debug_table:true
Label "Command:" position:10
TextArea height:100 position:10 width:480 cmd_input:true monospace:true
Button "Execute" position:330 action:exec_command default:true
Button "Cancel" position:410 action:close_dialog
Label "Directory:" position:10
TextField position:10 width:380 dir_input:true placeholder:"Leave empty for current"
Checkbox "Show hidden" id:show_hidden position:10
Checkbox "Long format" id:long_format position:150 checked:true
Button "List" position:230 action:exec_ls default:true
Button "Cancel" position:310 action:close_dialog
Label "Search Path:" position:10
TextField position:10 width:430 path_input:true value:.
Label "File Pattern:" position:10
TextField position:10 width:430 pattern_input:true placeholder:*.log
Checkbox "Case sensitive" id:case_sensitive position:10
Button "Find" position:280 action:exec_find default:true
Button "Cancel" position:360 action:close_dialog
Label "Pattern (regex):" position:10
TextField position:10 width:480 grep_pattern:true
Label "Input Source:" position:10
RadioGroup position:10 input_source:true
Radio "From variable" checked:true value:variable
Radio "From file" value:file
Radio "From last output" value:pipe
ComboBox position:150 width:200 items:$variables var_select:true visible:true
TextField position:150 width:200 file_select:true visible:false
Checkbox "Case insensitive" id:case_insensitive position:10
Checkbox "Invert match" id:invert_match position:150
Button "Search" position:340 action:exec_grep default:true
Button "Cancel" position:420 action:close_dialog
Label "Host:" position:10
TextField position:10 width:480 host_input:true placeholder:user@hostname
Label "Command:" position:10
TextArea height:100 position:10 width:480 ssh_cmd:true
Checkbox "Use key" id:use_key position:10 checked:true
TextField position:100 width:300 key_path:true placeholder:~/.ssh/id_rsa
Button "Connect" position:340 action:exec_ssh default:true
Button "Cancel" position:420 action:close_dialog
Label "SSH Host:" position:10
TextField position:10 width:530 backup_host:true
Label "Source Path:" position:10
TextField position:10 width:530 backup_src:true placeholder:/path/to/source
Label "Destination:" position:10
TextField position:10 width:530 backup_dst:true placeholder:/backup/location
GroupBox "Options" height:100 position:10 width:530
Checkbox "Compress" id:compress position:20 checked:true
Checkbox "Incremental" id:incremental position:150
Checkbox "Delete removed" id:delete_removed position:280
Label "Exclude:" position:20
TextField position:80 width:200 exclude_pattern:true placeholder:"*.tmp,*.log"
Button "Start Backup" position:380 action:exec_backup default:true
Button "Cancel" position:470 action:close_dialog
Label "API URL:" position:10
TextField position:10 width:480 api_url:true
Label "Search Pattern:" position:10
TextField position:10 width:480 api_pattern:true
Label "Check Interval (seconds):" position:10
SpinBox position:10 interval:true max:3600 min:10 value:60
GroupBox "Actions" height:120 position:10 width:480
Checkbox "Desktop notification" id:notify position:20 checked:true
Checkbox "Log to file" id:log_file position:20
TextField position:120 width:250 log_path:true placeholder:/tmp/monitor.log
Checkbox "Execute command" id:exec_on_match position:20
TextField position:120 width:250 match_cmd:true
Button "Start Monitor" position:330 action:start_monitor default:true
Button "Cancel" position:420 action:close_dialog

# Status bar at bottom
Panel height:25 position:bottom color:gray statusbar:true
Label "Ready" id:status_text position:5
Separator
Label "Variables: 0" id:var_count position:210
Separator
Label "History: 0" id:history_count position:310
ProgressBar id:progress position:right width:150 visible:false

# Dialog definitions for commands

Dialog "Execute Command" height:200 id:exec_dialog width:500

Dialog "List Files" height:150 id:ls_dialog width:400

Dialog "Find Files" height:200 id:find_dialog width:450

Dialog "Search Pattern" height:250 id:grep_dialog width:500

Dialog "SSH Command" height:300 id:ssh_dialog width:500

Dialog "Remote Backup" height:350 id:backup_dialog width:550

Dialog "API Monitor" height:400 id:monitor_dialog width:500

