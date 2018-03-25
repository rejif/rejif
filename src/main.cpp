
#include "mwindow.h"

#include <QApplication>

int main(int argc, char *argv[]){
    QApplication app(argc, argv);


    Q_INIT_RESOURCE(resource);

    MWindow w;
    w.show();

    return app.exec();
}
