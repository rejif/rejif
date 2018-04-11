#ifndef LAYOUT_H
#define LAYOUT_H

#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "slib.h"

class Layout : public QWidget{
public:
    QSize size;
    QImage image;
    Layout(QWidget *parent = 0):QWidget(parent){
        this->setMinimumHeight(30);
        image=QImage(720,30,QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::darkGray);
    }
protected:
    void paintEvent(QPaintEvent *event){
        QPainter painter(this);
        QRect dirtyRect = event->rect();
        painter.drawImage(dirtyRect, image, dirtyRect);
        painter.setPen(QPen(Qt::white));
        painter.drawText(10,20,QString("[%1]")
                .arg("PRAMS"));
    }
};

#endif // LAYOUT_H
