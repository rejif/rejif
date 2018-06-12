
#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow(){
    scribbleArea = new ScribbleArea;
    setCentralWidget(scribbleArea);
    createActions();
    createMenus();
    setWindowTitle(tr("Scribble"));
    resize(500, 500);
}










