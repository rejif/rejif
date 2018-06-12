
#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>

#include <QtWidgets>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif

class ScribbleArea : public QWidget{
    Q_OBJECT
public:
    ScribbleArea(QWidget *parent = 0):QWidget(parent){
        setAttribute(Qt::WA_StaticContents);
        modified = false;
        scribbling = false;
        myPenWidth = 1;
        myPenColor = Qt::blue;
    }
    bool openImage(const QString &fileName){
        QImage loadedImage;
        if (!loadedImage.load(fileName)){
            return false;
        }
        QSize newSize = loadedImage.size().expandedTo(size());
        resizeImage(&loadedImage, newSize);
        image = loadedImage;
        modified = false;
        update();
        return true;
    }
    bool saveImage(const QString &fileName, const char *fileFormat){
        QImage visibleImage = image;
        resizeImage(&visibleImage, size());
        if (visibleImage.save(fileName, fileFormat)) {
            modified = false;
            return true;
        } else {
            return false;
        }
    }
    void setPenColor(const QColor &newColor){
        myPenColor = newColor;
    }
    void setPenWidth(int newWidth){
        myPenWidth = newWidth;
    }
    bool isModified() const { return modified; }
    QColor penColor() const { return myPenColor; }
    int penWidth() const { return myPenWidth; }
public slots:
    void clearImage(){
        image.fill(qRgb(255, 255, 255));
        modified = true;
        update();
    }
    void print(){
#if QT_CONFIG(printdialog)
        QPrinter printer(QPrinter::HighResolution);
        QPrintDialog printDialog(&printer, this);
        if (printDialog.exec() == QDialog::Accepted) {
            QPainter painter(&printer);
            QRect rect = painter.viewport();
            QSize size = image.size();
            size.scale(rect.size(), Qt::KeepAspectRatio);
            painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
            painter.setWindow(image.rect());
            painter.drawImage(0, 0, image);
        }
#endif // QT_CONFIG(printdialog)
    }
protected:
    void mousePressEvent(QMouseEvent *event) override{
        if (event->button() == Qt::LeftButton) {
            lastPoint = event->pos();
            scribbling = true;
        }
    }
    void mouseMoveEvent(QMouseEvent *event) override{
        if ((event->buttons() & Qt::LeftButton) && scribbling)
            drawLineTo(event->pos());
    }
    void mouseReleaseEvent(QMouseEvent *event) override{
        if (event->button() == Qt::LeftButton && scribbling) {
            drawLineTo(event->pos());
            scribbling = false;
        }
    }
    void paintEvent(QPaintEvent *event) override{
        QPainter painter(this);
        QRect dirtyRect = event->rect();
        painter.drawImage(dirtyRect, image, dirtyRect);
    }
    void resizeEvent(QResizeEvent *event) override{
        if (width() > image.width() || height() > image.height()) {
            int newWidth = qMax(width() + 128, image.width());
            int newHeight = qMax(height() + 128, image.height());
            resizeImage(&image, QSize(newWidth, newHeight));
            update();
        }
        QWidget::resizeEvent(event);
    }
private:
    void drawLineTo(const QPoint &endPoint){
        QPainter painter(&image);
        painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
        painter.drawLine(lastPoint, endPoint);
        modified = true;
        int rad = (myPenWidth / 2) + 2;
        update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
        lastPoint = endPoint;
    }
    void resizeImage(QImage *image, const QSize &newSize){
        if (image->size() == newSize){
            return;
        }
        QImage newImage(newSize, QImage::Format_RGB32);
        newImage.fill(qRgb(255, 255, 255));
        QPainter painter(&newImage);
        painter.drawImage(QPoint(0, 0), *image);
        *image = newImage;
    }
    bool modified;
    bool scribbling;
    int myPenWidth;
    QColor myPenColor;
    QImage image;
    QPoint lastPoint;
};

#endif
