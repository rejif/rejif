
#include "mwindow.h"
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(resource);

    MWindow w;
    w.show();

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
