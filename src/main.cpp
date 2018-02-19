
#include "mwindow.h"

#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);


    Q_INIT_RESOURCE(resource);

    MWindow w;
    w.show();

    return a.exec();
}
