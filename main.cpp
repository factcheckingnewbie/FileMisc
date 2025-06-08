#include <QApplication>
#include "QtFileManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    try {
        QtFileManager window;
        window.show();
        
        return app.exec();
    } catch (const std::exception& e) {
        return -1;
    }
}
