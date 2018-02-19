
#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include "slib.h"
#include "image.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include <QWidget>
#include <QtWidgets>
#include <QColor>
#include <QPoint>

using namespace std;

enum PROFILE{ DEFAULT,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE };

enum Operation{
    UP,DOWN,RIGHT,LEFT,
    PLAYBLAST
};

class ScribbleArea : public QWidget{
    Q_OBJECT
public:
    ScribbleArea(QWidget *parent = 0): QWidget(parent){
        setAttribute(Qt::WA_StaticContents);
        setFixedSize(720,480);
        initialize();
        qDebug() << "Frame:" << getFrame() << "Layer:" << getLayer() << "Onion:" << getOnionSkinCount();
        qDebug() << images[frame][layer];
    }
    void initialize(){
        setAuthor("onoie");
        edit = false;
        scribbling = false;
        frame=0;
        images={{createImage(),createImage()}};
        layer=(images[frame].size()-1);
        onionSkinCount=1;
        createOnionSkin();
    }
    Image createImage(Qt::GlobalColor color=Qt::transparent){
        Image empty(720,480, QImage::Format_ARGB32_Premultiplied);
        empty.fill(color);
        return empty;
    }
    void createOnionSkin(){
        Image empty = createImage();
        if( 0 < getFrame() && 0 < getOnionSkinCount() ){
            QPixmap input = QPixmap::fromImage(getFrameImage(getFrame()-1));
            Image empty = createImage();
            QPainter p(&empty);
            p.setOpacity(0.2);
            p.drawPixmap(0, 0, input);
            p.end();
            setOnionSkin(QPixmap::fromImage(empty));
        }else{
            setOnionSkin(QPixmap::fromImage(empty));
        }
    }
    vector <Image> createImageLayer(int size=2){
        vector<Image> layer;
        for(int i = 0; i < size; i++) {
            layer.push_back(createImage());
        }
        return layer;
    }
    void addFrame(){
        images.push_back(createImageLayer());
        update();
    }
    void insertFrameAfter(){
        images.insert(images.begin()+getFrame()+1,createImageLayer());
        createOnionSkin();
        update();
    }
    bool loadImage(const QString &fileName){
        Image loadedImage;
        if (!loadedImage.load(fileName)){
            return false;
        }
        QSize newSize = loadedImage.size().expandedTo(size());
        resizeImage(&loadedImage, newSize);
        setImage(loadedImage);
        edit = false;
        createOnionSkin();
        update();
        return true;
    }
    bool saveImage(const QString &fileName, const char *fileFormat){
        Image visibleImage = getFrameImage(getFrame());
        resizeImage(&visibleImage, size());
        if (visibleImage.save(fileName, fileFormat)) {
            edit = false;
            return true;
        } else {
            return false;
        }
    }
    QString getTitle(){ return this->title; }
    void setTitle(QString title){ this->title=title; }
    QString getAuthor(){ return this->author; }
    void setAuthor(QString author){ this->author=author; }
    bool isEdit() const { return edit; }
    void editing(){ edit=true; }
    QColor getPenColor() const { return penColor; }
    void setPenColor(const QColor &newColor){ penColor = newColor; }
    int getPenWidth() const { return penWidth; }
    void setPenWidth(int newWidth){ penWidth = newWidth; }
    unsigned int getFrame() const { return frame; }
    void setFrame(int frame){ this->frame = frame; }
    unsigned int getLayer() const { return layer; }
    void setLayer(int layer){ this->layer = layer; }
    Image getImage(){ return this->images[frame][layer]; }
    Image* getImagep(){ return &images[frame][layer]; }
    void setImage(Image image){ this->images[frame][layer]=image; }
    Image getLayerImage(int layer){ return images[frame][layer]; }
    void setLayerImage(int layer,Image image){ this->images[frame][layer]=image; }
    Image getFrameLayerImage(int frame,int layer){ return images[frame][layer]; }
    void setFrameLayerImage(int frame,int layer,Image image){ this->images[frame][layer]=image; }
    Image getFrameImage(unsigned int frame){
        vector <Image> frameImages = images[frame];
        Image frameImage = createImage();
        QPainter imagePainter(&frameImage);
        for(auto img = frameImages.begin(); img != frameImages.end(); ++img) {
            imagePainter.setCompositionMode(img->getCompositionMode());
            imagePainter.drawImage(0,0,*img);
        }
        return frameImage;
    }
    vector <vector <Image>> getImages(){ return this->images; }
    QPixmap getOnionSkin(){ return onionSkin;}
    void setOnionSkin(QPixmap onionSkin){ this->onionSkin=onionSkin; }
    unsigned int getOnionSkinCount(){ return onionSkinCount;}
    void setOnionSkinCountPlus(){ onionSkinCount++; }
    void setOnionSkinCountMinus(){ if( 0 < onionSkinCount ){ onionSkinCount--; } }
    Image getDisplayImage(unsigned int frame,Qt::GlobalColor bg=Qt::transparent){
        Image frameImage = createImage(bg);
        QPainter framePainter(&frameImage);
        framePainter.drawImage(0,0,getFrameImage(frame));
        framePainter.drawImage(frameImage.width()-sign.width()-5,frameImage.height()-sign.height()-5,sign);
        //Navi
        framePainter.drawImage(0,0,guide);
        framePainter.drawText(frameImage.width()-80,25,
            QString("[%1/%2]").arg(frame,3,10,QChar('0')).arg(getImages().size()-1,3,10,QChar('0')));
        framePainter.drawText(10,frameImage.height()-15,
            QString("[author:%2]").arg(getAuthor()));
        return frameImage;
    }
signals:
    void updateStatusbarSignal(QString);
public slots:
    void test(){
        qDebug()<<"ScribbleareaTest";
    }
    void action(Operation op){
        if( op==Operation::UP && layer < (images[frame].size()-1) ){
            layer++;
        }else if( op==Operation::DOWN && 0 < layer ){
            layer--;
        }else if( op==Operation::RIGHT && frame < (images.size()-1) ){
            frame++;
        }else if( op==Operation::LEFT && 0 < frame ){
            frame--;
        }else if( op==Operation::PLAYBLAST ){
            frame < (images.size()-1) ? frame++:frame=0;
        }
        createOnionSkin();
        QString frameStr("[");
        for (unsigned int i = 0; i < getImages().size(); i++){
            frameStr+=(frame==i?"#":"-");
        }
        frameStr+="]";
        emit updateStatusbarSignal(
            QString("(Frame:%1,Layer:%2) @ %3").arg(frameStr).arg(layer).arg(Slib::getNow())
        );
        update();
    }
    void clearImage(){
        getImagep()->fill(qRgba(255,255,255,0));
        editing();
        createOnionSkin();
        update();
    }
private:
    QImage sign = QImage(":onoie.png").scaled(35,35,Qt::KeepAspectRatio,Qt::FastTransformation);
    //QPixmap pixmap = QPixmap::fromImage(QImage(""));
    QPixmap onionSkin;
    QString title="untitled",author="unknown";
    unsigned int frame,layer,onionSkinCount;
    vector<vector<Image>> images;
    bool edit;
    bool scribbling;
    int penWidth=10;
    QColor penColor=Qt::red;
    QPoint lastPoint;
    Image guide = Image(":guide.png");
    void drawLineTo(const QPoint &endPoint){
        QPainter painter(getImagep());
        painter.setCompositionMode(QPainter::CompositionMode_Source);//forErase
        painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
        painter.drawLine(lastPoint, endPoint);
        editing();
        int rad = (penWidth / 2) + 2;
        update(QRect(lastPoint, endPoint)
               .normalized()
               .adjusted(-rad, -rad, +rad, +rad));
        lastPoint = endPoint;
    }
    void resizeImage(Image *image, const QSize &newSize){
        if (image->size() == newSize){
            return;
        }
        Image newImage(newSize, Image::Format_RGB32);
        newImage.fill(qRgb(255, 255, 255));
        QPainter painter(&newImage);
        painter.drawImage(QPoint(0, 0), *image);
        *image = newImage;
    }
protected:
    void paintEvent(QPaintEvent *event) override{
        QPainter painter(this);
        QRect dirtyRect = event->rect();
        painter.setRenderHint(QPainter::Antialiasing,false);
        //TransparentBackgroundImage
        painter.drawImage(dirtyRect,createImage(Qt::white),dirtyRect);
        painter.drawPixmap(dirtyRect,getOnionSkin(),dirtyRect);
        painter.drawImage(dirtyRect,getDisplayImage(getFrame()),dirtyRect);
        painter.setBackground(QBrush(Qt::white));
        painter.drawText(10,25,
            QString("[Frame:%1/%2,Layer:%3/%4@%5]")
                .arg(frame,3,10,QChar('0'))
                .arg(getImages().size()-1,3,10,QChar('0'))
                .arg(layer,2,10,QChar('0'))
                .arg(getImages()[frame].size()-1,2,10,QChar('0'))
                .arg(layer==0?"Back":"Front"));
    }
    void mousePressEvent(QMouseEvent *event) override{
        if (event->button() == Qt::LeftButton) {
            lastPoint = event->pos();
            scribbling = true;
        }
    }
    void mouseMoveEvent(QMouseEvent *event) override{
        if ((event->buttons() & Qt::LeftButton) && scribbling){
            drawLineTo(event->pos());
        }
    }
    void mouseReleaseEvent(QMouseEvent *event) override{
        if (event->button() == Qt::LeftButton && scribbling) {
            drawLineTo(event->pos());
            scribbling = false;
        }
    }
    void resizeEvent(QResizeEvent *event) override{
        if (width() > getImage().width() || height() > getImage().height()) {
            int newWidth = qMax(width() + 128, getImage().width());
            int newHeight = qMax(height() + 128, getImage().height());
            resizeImage(getImagep(), QSize(newWidth, newHeight));
            update();
        }
        QWidget::resizeEvent(event);
    }
};


#endif
