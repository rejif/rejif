#ifndef INFO_H
#define INFO_H

#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "slib.h"

class Info : public QWidget{
public:
    Info(QWidget *parent = 0):QWidget(parent){
        this->setMinimumSize(200,200);
        image=QImage(200,200,QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::darkGray);
    }
    QImage image;
protected:
    void paintEvent(QPaintEvent *event){
        QPainter painter(this);
        QRect dirtyRect = event->rect();
        painter.drawImage(dirtyRect, image, dirtyRect);
        painter.setPen(QPen(Qt::white));
        painter.drawText(10,25,QString("[%1]")
                .arg("PRAMS"));
    }
};

#endif // INFO_H
