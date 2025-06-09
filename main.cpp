#include <QApplication>
#include "filemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    FileManager window;
    window.show();
    
    return app.exec();
}
