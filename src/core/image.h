#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QPainter>

class Image : public QImage{
public:
    explicit Image():QImage(){}
    explicit Image(const QString &fileName, const char *format = Q_NULLPTR)
        :QImage(fileName,format){}
    explicit Image(const QSize &size, Format format)
        :QImage(size,format){}
    explicit Image(int width, int height, Format format)
        :QImage(width,height,format){}
    explicit Image(uchar *data, int width, int height, Format format, QImageCleanupFunction cleanupFunction = Q_NULLPTR, void *cleanupInfo = Q_NULLPTR)
        :QImage(data,width,height,format,cleanupFunction,cleanupInfo){}
    explicit Image(const uchar *data, int width, int height, Format format, QImageCleanupFunction cleanupFunction = Q_NULLPTR, void *cleanupInfo = Q_NULLPTR)
        :QImage(data,width,height,format,cleanupFunction,cleanupInfo){}
    explicit Image(uchar *data, int width, int height, int bytesPerLine, Format format, QImageCleanupFunction cleanupFunction = Q_NULLPTR, void *cleanupInfo = Q_NULLPTR)
        :QImage(data,width,height,bytesPerLine,format,cleanupFunction,cleanupInfo){}
    explicit Image(const uchar *data, int width, int height, int bytesPerLine, Format format, QImageCleanupFunction cleanupFunction = Q_NULLPTR, void *cleanupInfo = Q_NULLPTR)
        :QImage(data,width,height,bytesPerLine,format,cleanupFunction,cleanupInfo){}
    QPainter::CompositionMode getCompositionMode(){ return mode; }
    void setCompositionMode(QPainter::CompositionMode mode){ this->mode = mode; }
private:
    QPainter::CompositionMode mode = QPainter::CompositionMode_SourceOver;
};

#endif // IMAGE_H
