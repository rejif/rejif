
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QDir>
#include <QAction>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImageWriter>
#include <QMenu>
#include <QMenuBar>

#include "scribblearea.h"

class ScribbleArea;

class MainWindow:public QMainWindow{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0):QMainWindow(parent){
        setWindowTitle(tr("Scribble"));
    }
    virtual void initialize(int w=500,int h=500){
        scribbleArea = new ScribbleArea;
        setCentralWidget(scribbleArea);
        createMenus();
        resize(w,h);
    }
    virtual void createMenus(){
        //FileMenu
        QMenu *fileMenu = new QMenu(tr("&File"), this);
        QAction *openAct = new QAction(tr("&Open..."), this);
        openAct->setShortcuts(QKeySequence::Open);
        connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
        fileMenu->addAction(openAct);
        QMenu *saveAsMenu = new QMenu(tr("&Save As"), this);
        QList<QAction *> saveAsActs;
        foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
            QString text = tr("%1...").arg(QString(format).toUpper());
            QAction *action = new QAction(text, this);
            action->setData(format);
            connect(action, SIGNAL(triggered()), this, SLOT(save()));
            saveAsActs.append(action);
        }
        foreach (QAction *action, saveAsActs){
            saveAsMenu->addAction(action);
        }
        fileMenu->addMenu(saveAsMenu);
        QAction *printAct = new QAction(tr("&Print..."), this);
        connect(printAct, SIGNAL(triggered()), scribbleArea, SLOT(print()));
        fileMenu->addAction(printAct);
        fileMenu->addSeparator();
        QAction *exitAct = new QAction(tr("E&xit"), this);
        exitAct->setShortcuts(QKeySequence::Quit);
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
        fileMenu->addAction(exitAct);
        menuBar()->addMenu(fileMenu);
        //OptionMenu
        QMenu *optionMenu = new QMenu(tr("&Options"), this);
        QAction *penColorAct = new QAction(tr("&Pen Color..."), this);
        connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));
        optionMenu->addAction(penColorAct);
        QAction *penWidthAct = new QAction(tr("Pen &Width..."), this);
        connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));
        optionMenu->addAction(penWidthAct);
        optionMenu->addSeparator();
        QAction *clearScreenAct = new QAction(tr("&Clear Screen"), this);
        clearScreenAct->setShortcut(tr("Ctrl+L"));
        connect(clearScreenAct, SIGNAL(triggered()),scribbleArea, SLOT(clearImage()));
        optionMenu->addAction(clearScreenAct);
        menuBar()->addMenu(optionMenu);
        //HelpMenu
        QMenu *helpMenu = new QMenu(tr("&Help"), this);
        QAction *aboutAct = new QAction(tr("&About"), this);
        connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
        helpMenu->addAction(aboutAct);
        QAction *aboutQtAct = new QAction(tr("About &Qt"), this);
        connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
        helpMenu->addAction(aboutQtAct);
        menuBar()->addMenu(helpMenu);
    }
protected:
    void closeEvent(QCloseEvent *event) override{
        maybeSave()?event->accept():event->ignore();
    }
private slots:
    void open(){
        if (maybeSave()) {
            QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"), QDir::currentPath());
            if (!fileName.isEmpty()){
                scribbleArea->openImage(fileName);
            }
        }
    }
    void save(){
        QAction *action = qobject_cast<QAction *>(sender());
        QByteArray fileFormat = action->data().toByteArray();
        saveFile(fileFormat);
    }
    void penColor(){
        QColor newColor = QColorDialog::getColor(scribbleArea->penColor());
        if (newColor.isValid()){
            scribbleArea->setPenColor(newColor);
        }
    }
    void penWidth(){
        bool ok;
        int newWidth = QInputDialog::getInt(this, tr("Scribble"),tr("Select pen width:"),scribbleArea->penWidth(),1, 50, 1, &ok);
        if (ok){
            scribbleArea->setPenWidth(newWidth);
        }
    }
    void about(){
        QMessageBox::about(this, tr("About Scribble"),
       tr("<p>The <b>Scribble</b> example shows how to use QMainWindow as the "
          "base widget for an application, and how to reimplement some of "
          "QWidget's event handlers to receive the events generated for "
          "the application's widgets:</p><p> We reimplement the mouse event "
          "handlers to facilitate drawing, the paint event handler to "
          "update the application and the resize event handler to optimize "
          "the application's appearance. In addition we reimplement the "
          "close event handler to intercept the close events before "
          "terminating the application.</p><p> The example also demonstrates "
          "how to use QPainter to draw an image in real time, as well as "
          "to repaint widgets.</p>"));
    }
private:
    ScribbleArea *scribbleArea;
    bool maybeSave(){
        if (scribbleArea->isModified()) {
            QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Scribble"),
               tr("The image has been modified.\n"
                  "Do you want to save your changes?"),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            if (ret == QMessageBox::Save) {
                return saveFile("png");
            } else if (ret == QMessageBox::Cancel) {
                return false;
            }
        }
        return true;
    }
    bool saveFile(const QByteArray &fileFormat){
        QString initialPath = QDir::currentPath() + "/untitled." + fileFormat;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),initialPath,tr("%1 Files (*.%2);;All Files (*)").arg(QString::fromLatin1(fileFormat.toUpper())).arg(QString::fromLatin1(fileFormat)));
        if (fileName.isEmpty()) {
            return false;
        } else {
            return scribbleArea->saveImage(fileName, fileFormat.constData());
        }
    }
};

#endif
