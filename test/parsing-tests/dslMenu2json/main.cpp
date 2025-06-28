#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "dsl_parser.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
     QString dslContent;
//     QString dslContent = R"(
// Window "File Manager" width:800 height:600
// 
// Panel menubar position:top color:green
//     FoldableMenu "File"
//         Item "Open File" action:open_file shortcut:Ctrl+O icon:file-open
//         Item "Save" action:save_file shortcut:Ctrl+S icon:file-save
//         Item "Save As" action:save_as_file shortcut:Ctrl+Shift+S icon:file-save-as
//         Item "Close" action:close_file shortcut:Ctrl+W icon:file-close
//         Separator
//         Item "Exit" action:exit_app shortcut:Ctrl+Q icon:app-exit
//     FoldableMenu "Edit"
//         Item "Undo" action:undo shortcut:Ctrl+Z icon:edit-undo
//         Item "Redo" action:redo shortcut:Ctrl+Y icon:edit-redo
//         Separator
//         Item "Cut" action:cut shortcut:Ctrl+X icon:edit-cut
//         Item "Copy" action:copy shortcut:Ctrl+C icon:edit-copy
//         Item "Paste" action:paste shortcut:Ctrl+V icon:edit-paste
//     FoldableMenu "View"
//         Item "Show Sidebar" action:toggle_sidebar checked:true
//         Item "Show Preview" action:toggle_preview checked:true
//     FoldableMenu "Tools"
//         Item "List Files" action:list_files shortcut:Ctrl+L icon:list
//         Item "Find Pattern" action:find_pattern shortcut:Ctrl+F icon:search
//         Item "Count Matches" action:count_matches shortcut:Ctrl+M icon:numbers
//     FoldableMenu "Help"
//         Item "About" action:show_about
//         Item "Help" action:show_help shortcut:F1
// 
// Panel splitter orientation:horizontal sizes:300,500
// 
// Panel buttonbar position:top color:lightgray
//     Button "Open" action:open_file icon:file-open
//     Button "Save" action:save_file icon:file-save
//     Button "Copy" action:copy icon:edit-copy
//     Button "Paste" action:paste icon:edit-paste
//     Button "About" action:show_about icon:help
// )";
//     

if (argc > 1) {
    QFile file(argv[1]);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        dslContent = stream.readAll();
    }
}
    // Parse DSL
    DSLParser parser;
    QJsonDocument json = parser.parse(dslContent);
    
    if (!parser.success()) {
        auto error = parser.lastError();
        qCritical() << "Parse error at line" << error->line 
                    << "column" << error->column 
                    << ":" << error->message;
        return 1;
    }
    
    // Output JSON
    qDebug().noquote() << "Generated JSON:";
    qDebug().noquote() << json.toJson(QJsonDocument::Indented);
    
    // Save to file
    QFile outFile("output.json");
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&outFile);
        stream << json.toJson(QJsonDocument::Indented);
        outFile.close();
        qDebug() << "JSON saved to output.json";
    }
    
    return 0;
}
