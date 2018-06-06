#ifndef MONITOR_H
#define MONITOR_H

#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "../slib.h"

class Monitor : public QWidget{
public:
    QSize size;
    QImage image;
    Monitor(QWidget *parent = 0):QWidget(parent){
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
        painter.drawText(10,20,QString("Monitor[%1]").arg("PRAMS"));
    }
};

#endif // MONITOR_H
