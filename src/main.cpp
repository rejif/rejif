
#include "mwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include "base/mainwindow.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(resource);

    QRect aa(QApplication::desktop()->availableGeometry());
    //MainWindow mainWindow;
    //mainWindow.initialize();
    //mainWindow.show();
    MWindow mw;
    mw.initialize(1200,700);
    mw.show();
    mw.move(aa.width()/2-mw.width()/2,aa.height()/2-mw.height()/2);

    return app.exec();
}
