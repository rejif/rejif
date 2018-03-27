#ifndef PREV_H
#define PREV_H

#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "slib.h"

class Preview : public QWidget{
public:
    Preview(QWidget *parent = 0):QWidget(parent){
        this->setMinimumSize(200,130);
        image=QImage(200,130,QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::darkGray);
    }
    QImage image;
protected:
    void paintEvent(QPaintEvent *event){
        QPainter painter(this);
        QRect dirtyRect = event->rect();
        painter.drawImage(dirtyRect, image, dirtyRect);
    }
};

#endif // PREV_H
