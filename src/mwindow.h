#ifndef MWINDOW_H
#define MWINDOW_H

#include "slib.h"
#include "base/mainwindow.h"
#include "core/image.h"
#include "core/scene.h"
#include "widget/monitor.h"
#include "widget/info.h"
#include "widget/preview.h"

#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QMainWindow>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImageWriter>
#include <QProcess>

class MWindow:public MainWindow{
    Q_OBJECT
public:
    MWindow(QWidget *parent = 0):MainWindow(parent){
        setWindowTitle(tr("Rejif"));
        setWindowFlags(Qt::WindowStaysOnTopHint);
    }
    ~MWindow(){}
    void initialize(int w,int h)override{
        scene = new Scene(this);

        createWidget();
        createMenus();
        createStatusBar();

        QObject::connect(
            scene,SIGNAL(updateStatusbarSignal(QString)),
            this,SLOT(updateStatusText(QString))
        );
        updateStatusText(QString("Initialized %1x%2 in %3x%4")
            .arg(this->width())
            .arg(this->height())
            .arg(scene->width())
            .arg(scene->height()));

        useDefaultTool();

        setMaximumWidth(w);
        setMaximumHeight(h);
        resize(this->minimumWidth(),this->minimumHeight());
    }
    void createMenus()override{
        //FileMenu
        QMenu *fileMenu = new QMenu(tr("&File"), this);
        QAction *openAct = new QAction(tr("&Open..."), this);
        openAct->setShortcuts(QKeySequence::Open);
        connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
        foreach (QByteArray format, QImageWriter::supportedImageFormats()) {
            QString extension = tr("%1").arg(QString(format).toUpper());
            //qDebug() << extension;
            QStringList support;
            support << "JPEG" << "PNG" << "BMP";
            switch(support.indexOf(extension)){
            case 0:
            case 1:
            case 2:
                QAction *action = new QAction(extension, this);
                action->setData(format);
                connect(action, SIGNAL(triggered()), this, SLOT(save()));
                saveAsActs.append(action);
                break;
            }
        }
        fileMenu->addAction(openAct);

        QMenu *saveAsMenu = new QMenu(tr("&Save As"), this);
        foreach (QAction *action, saveAsActs){
            saveAsMenu->addAction(action);
        }
        fileMenu->addMenu(saveAsMenu);

        fileMenu->addAction(Slib::createLambdaAction("QuietSave",[=]{
            Image img = scene->getFrameImage(scene->getFrame());
            QString imgPath = QString("%1_%2.%3")
                    .arg(scene->getTitle())
                    .arg(Slib::getNow("yyyyMMdd_hhmmss"))
                    .arg("png");
            saveImage(img,cacheDir+imgPath);
        })
                            );
        fileMenu->addSeparator();

        QAction *exitAct = new QAction(tr("E&xit"), this);
        exitAct->setShortcuts(QKeySequence::Quit);
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
        fileMenu->addAction(exitAct);
        menuBar()->addMenu(fileMenu);

        //SequenceMenu
        QMenu *sequenceMenu = new QMenu(tr("&Sequence"), this);
        menuBar()->addMenu(sequenceMenu);
        sequenceMenu->addAction(createSequenceLoadAction());
        sequenceMenu->addAction(createFrameSequenceSaveAction());
        sequenceMenu->addAction(createLayerSequenceSaveAction());
        sequenceMenu->addAction(createPublishAction());

        //OptionMenu
        QMenu *optionMenu = new QMenu(tr("&Options"), this);
        menuBar()->addMenu(optionMenu);
        QAction *penColorAct= new QAction(tr("&Pen Color..."), this);
        connect(penColorAct, SIGNAL(triggered()), this, SLOT(penColor()));
        optionMenu->addAction(penColorAct);
        QAction *penWidthAct = new QAction(tr("Pen &Width..."), this);
        connect(penWidthAct, SIGNAL(triggered()), this, SLOT(penWidth()));
        optionMenu->addAction(penWidthAct);
        optionMenu->addSeparator();
        QAction *clearScreenAct = new QAction(tr("&Clear Screen"), this);
        clearScreenAct->setShortcut(tr("Ctrl+L"));
        connect(clearScreenAct, SIGNAL(triggered()),scene, SLOT(clearImage()));
        optionMenu->addAction(clearScreenAct);

        //LambdaMenu
        QMenu *lambdaMenu = new QMenu(tr("&Lambda"), this);
        menuBar()->addMenu(lambdaMenu);
        lambdaMenu->addAction(Slib::createLambdaAction("OnionSkinPlus",[=]{
            scene->setOnionSkinCountPlus();
        }));
        lambdaMenu->addAction(Slib::createLambdaAction("OnionSkinMinus",[=]{
            scene->setOnionSkinCountMinus();
        }));
        lambdaMenu->addAction(Slib::createLambdaAction("MergeFrontToBack",[=]{
            Image base = scene->getLayerImage(0);
            QPainter mergePainter(&base);
            mergePainter.drawImage(0,0,scene->getLayerImage(1));
            scene->setLayerImage(1,scene->createImage());
            scene->setLayerImage(0,base);
        }));
        lambdaMenu->addAction(Slib::createLambdaAction("MergeFrontToBackAll",[=]{
            vector < vector <Image>> images = scene->getImages();
            for(int frame = 0; frame < (int) images.size(); ++frame) {
                Image base = scene->getFrameLayerImage(frame,0);
                QPainter mergePainter(&base);
                mergePainter.drawImage(0,0,scene->getFrameLayerImage(frame,1));
                scene->setFrameLayerImage(frame,1,scene->createImage());
                scene->setFrameLayerImage(frame,0,base);
            }
        }));
        lambdaMenu->addAction(Slib::createLambdaAction("MergeBackToFront",[=]{
            Image base = scene->getLayerImage(1);
            QPainter mergePainter(&base);
            mergePainter.drawImage(0,0,scene->getLayerImage(0));
            scene->setLayerImage(0,scene->createImage());
            scene->setLayerImage(1,base);
        }));
        lambdaMenu->addAction(Slib::createLambdaAction("MergeBackToFrontAll",[=]{
            vector < vector <Image>> images = scene->getImages();
            for(int frame = 0; frame < (int) images.size(); ++frame) {
                Image base = scene->getFrameLayerImage(frame,1);
                QPainter mergePainter(&base);
                mergePainter.drawImage(0,0,scene->getFrameLayerImage(frame,0));
                scene->setFrameLayerImage(frame,0,scene->createImage());
                scene->setFrameLayerImage(frame,1,base);
            }
        }));

        //ShortcutMenu
        QMenu *shortcutMenu = new QMenu(tr("&Shortcut"), this);
        menuBar()->addMenu(shortcutMenu);
        //T
        QAction *tAct = new QAction(tr("&TKeyAction [PrimaryTool]"));
        tAct->setShortcut(tr("T"));
        shortcutMenu->addAction(tAct);
        connect(tAct,SIGNAL(triggered(bool)),this,SLOT(usePrimary()));
        //R
        QAction *rAct = new QAction(tr("&RKeyAction [SecondaryTool]"));
        rAct->setShortcut(tr("R"));
        shortcutMenu->addAction(rAct);
        connect(rAct,SIGNAL(triggered(bool)),this,SLOT(useSecondary()));
        //E
        QAction *eAct = new QAction(tr("&EKeyAction [TertiaryTool]"));
        eAct->setShortcut(tr("E"));
        shortcutMenu->addAction(eAct);
        connect(eAct,SIGNAL(triggered(bool)),this,SLOT(useTertiary()));

        menuBar()->addSeparator();

        //Q
        QAction *qAct = Slib::createLambdaAction(tr("&QKeyAction[Add Frame]"),[=]{
            scene->addFrame();
            updateStatusText(QString("add frame [0-%1]").arg(scene->getImages().size()-1));
        });
        qAct->setShortcut(Qt::Key_Q);
        shortcutMenu->addAction(qAct);
        //W
        QAction *wAct = Slib::createLambdaAction(tr("&WKeyAction [LayerUp]"),[=]{
            scene->action(Operation::UP);
        });
        wAct->setShortcut(tr("W"));
        shortcutMenu->addAction(wAct);
        //S
        QAction *sAct = Slib::createLambdaAction(tr("&SKeyAction [LayerDown]"),[=]{
            scene->action(Operation::DOWN);
        });
        sAct->setShortcut(Qt::Key_S);
        shortcutMenu->addAction(sAct);
        //D
        QAction *dAct = Slib::createLambdaAction(tr("&DKeyAction [FrameRight]"),[=]{
            scene->action(Operation::RIGHT);
        });
        dAct->setShortcut(Qt::Key_D);
        shortcutMenu->addAction(dAct);
        //A
        QAction *aAct = Slib::createLambdaAction(tr("&AKeyAction [FrameLeft]"),[=]{
            scene->action(Operation::LEFT);
        });
        aAct->setShortcut(Qt::Key_A);
        shortcutMenu->addAction(aAct);
        //Z
        QAction *zAct = Slib::createLambdaAction(tr("&ZKeyAction [PLAYBLAST]"),[=]{
            scene->action(Operation::PLAYBLAST);
        });
        zAct->setShortcut(Qt::Key_Z);
        shortcutMenu->addAction(zAct);

        //ProfileMenu
        QMenu *profileMenu = new QMenu(tr("&Profile"), this);
        menuBar()->addMenu(profileMenu);
        //1
        QAction *act1 = Slib::createLambdaAction(tr("&Profile No.1"),[=]{
            updateStatusText("setProfile:1");
            t_profile=PROFILE::ONE;
        });
        act1->setShortcut(Qt::Key_1);
        profileMenu->addAction(act1);
        //2
        QAction *act2 = Slib::createLambdaAction(tr("&Profile No.2"),[=]{
            updateStatusText("setProfile:2");
            t_profile=PROFILE::TWO;
        });
        act2->setShortcut(Qt::Key_2);
        profileMenu->addAction(act2);
        //3
        QAction *act3 = Slib::createLambdaAction(tr("&Profile No.3"),[=]{
            updateStatusText("setProfile:3");
            t_profile=PROFILE::THREE;
        });
        act3->setShortcut(Qt::Key_3);
        profileMenu->addAction(act3);
        //4
        QAction *act4 = Slib::createLambdaAction(tr("&Profile No.4"),[=]{
            updateStatusText("setProfile:4");
            t_profile=PROFILE::FOUR;
        });
        act4->setShortcut(Qt::Key_4);
        profileMenu->addAction(act4);
        //5
        QAction *act5 = Slib::createLambdaAction(tr("&Profile No.5"),[=]{
            updateStatusText("setProfile:5");
            t_profile=PROFILE::FIVE;
        });
        act5->setShortcut(Qt::Key_5);
        profileMenu->addAction(act5);
        //0
        QAction *act0 = new QAction(tr("&Profile No.0"));
        act0->setShortcut(Qt::Key_0);
        profileMenu->addAction(act0);
        connect(act0,SIGNAL(triggered(bool)),this,SLOT(useDefaultTool()));

        //HelpMenu
        QMenu *helpMenu = new QMenu(tr("&Help"), this);
        QAction *aboutQtAct = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
        aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));

        QAction* updateAct = new QAction("ApplicationUpdate");
        QObject::connect(updateAct,&QAction::triggered,[=]{
            QProcess::execute("C:/Windows/System32/cmd.exe /C start https://ci.appveyor.com/project/onoie/rejif/build/artifacts/");
        });
        helpMenu->addAction(updateAct);

        menuBar()->addMenu(helpMenu);
    }
    const QString cacheDir = Slib::mkdirp(QDir::currentPath()+"/cache/");
    const QString frameDir = Slib::mkdirp(QDir::currentPath()+"/frame/");
    const QString layerDir = Slib::mkdirp(QDir::currentPath()+"/layer/");
    PROFILE t_profile = PROFILE::DEFAULT;
protected:
    void closeEvent(QCloseEvent *event) override{
        event->accept();//->ignore();
    }
public slots:
    void useDefaultTool(){
        updateStatusText("useProfile:Default");
        t_profile=PROFILE::DEFAULT;
        usePrimary();
    }
    void usePrimary(){
        updateStatusText("usePrimaryTool");
        switch (t_profile) {
            case PROFILE::ONE:
                scene->setPenWidth(2);
                scene->setPenColor(QColor(0,0,0,255));
                break;
            case PROFILE::TWO:
                scene->setPenWidth(5);
                scene->setPenColor(QColor(191,191,191,255));
                break;
            case PROFILE::THREE:
                scene->setPenWidth(20);
                scene->setPenColor(QColor(191,191,191,255));
                break;
            case PROFILE::FOUR:
                scene->setPenWidth(5);
                scene->setPenColor(QColor(250,241,230,255));
                break;
            case PROFILE::FIVE:
                scene->setPenWidth(5);
                scene->setPenColor(QColor(54,46,43,255));
                break;
            case PROFILE::SIX:
            case PROFILE::SEVEN:
            case PROFILE::EIGHT:
            case PROFILE::NINE:
            case PROFILE::DEFAULT:
            default:
                qDebug()<<"DefaultPrimary";
                scene->setPenWidth(1);
                scene->setPenColor(Qt::black);
                break;
        }
    }
    void useSecondary(){
        updateStatusText("useSecondaryTool");
        switch (t_profile) {
            case PROFILE::THREE:
                scene->setPenWidth(16);
                scene->setPenColor(QColor(Qt::transparent));
                break;
            case PROFILE::ONE:
            case PROFILE::TWO:
            case PROFILE::FOUR:
            case PROFILE::FIVE:
            case PROFILE::SIX:
            case PROFILE::SEVEN:
            case PROFILE::EIGHT:
            case PROFILE::NINE:
            case PROFILE::DEFAULT:
            default:
                qDebug()<<"useDefaultSecondary";
                scene->setPenWidth(8);
                scene->setPenColor(QColor(Qt::transparent));
                break;
        }
    }
    void useTertiary(){
        updateStatusText("useTertiaryTool");
        switch (t_profile) {
            case PROFILE::ONE:
            case PROFILE::TWO:
            case PROFILE::THREE:
            case PROFILE::FOUR:
            case PROFILE::FIVE:
            case PROFILE::SIX:
            case PROFILE::SEVEN:
            case PROFILE::EIGHT:
            case PROFILE::NINE:
            case PROFILE::DEFAULT:
            default:
                qDebug()<<"useDefaultTertiary";
                break;
        }
    }
private slots:
    void open(){
        QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty()){
            scene->loadImage(fileName);
        }
    }
    void save(){
        QAction *action = qobject_cast<QAction *>(sender());
        QByteArray fileFormat = action->data().toByteArray();
        saveFile(fileFormat);
    }
    void penColor(){
        QColor newColor = QColorDialog::getColor(scene->getPenColor());
        if (newColor.isValid())
            scene->setPenColor(newColor);
    }
    void penWidth(){
        bool ok;
        int newWidth = QInputDialog::getInt(this,
            tr("Scribble"),
            tr("Select pen width:"),
            scene->getPenWidth(),
            1, 50, 1, &ok);
        if (ok){
            scene->setPenWidth(newWidth);
        }
    }
    void updateStatusText(QString text){
        statusText->setText(text);
    }
private:
    Scene *scene;
    QList<QAction *> saveAsActs;
    QLabel *statusText;

    QString createFileName(QString title,int frame,int layer,QString extension="png"){
        return QString("%1-%2#%3.%4")
            .arg(title)
            .arg(frame,3,10,QChar('0'))
            .arg(layer,2,10,QChar('0'))
            .arg(extension);
    }
    void saveImage(Image img,QString path){
        qDebug()<<"SaveImage:" << path;
        if(!img.save(path,"PNG")){
            QMessageBox::warning(this,
                tr("Warning"),
               tr("SequenceSaveWarning"),
               QMessageBox::Ok);
        }
    }
    QAction* createSequenceLoadAction(){
        return Slib::createLambdaAction("SequenceLoad",[=]{
            scene->initialize();
            unsigned int frame=0,layer=0;
            bool notfound=false;
            QString filePath = QFileDialog::getOpenFileName(this,tr("Open File"),layerDir );
            QRegExp reg("(^.+)/(.+)-(.+)#(.+).(.+)");
            if(reg.exactMatch(filePath)){
                reg.lastIndexIn(filePath);
                QString dir=reg.cap(1);
                QString name=reg.cap(2);
                while(true){
                    QString imgPath = dir+"/"+createFileName(name,frame,layer);
                    qDebug()<< imgPath ;
                    Image loadedImage;
                    if (!loadedImage.load(imgPath)){
                        if(notfound){
                            //NotFound
                            break;
                        }
                        notfound=true;
                        frame++;
                        layer=0;
                    }else{
                        if(frame > scene->getImages().size()-1){
                            //qDebug()<<sequence<<scribbleArea->getImages().size();
                            scene->addFrame();
                        }
                        scene->setFrameLayerImage(frame,layer,loadedImage);
                        notfound=false;
                        layer++;
                    }
                }
                scene->createOnionSkin();
                scene->update();
            }else{
                //Error
            }
            qDebug()<<"load complete";
        });
    }
    QAction* createFrameSequenceSaveAction(){
        return Slib::createLambdaAction("FrameSequenceSave",[=]{
            vector < vector <Image>> images = scene->getImages();
            for(int frame = 0; frame < (int) images.size(); ++frame) {
                Image displayImage = scene->getFrameImage(frame);
                QString imgPath = createFileName(scene->getTitle(),frame,0);
                saveImage(displayImage,frameDir+imgPath);
            }
        });
    }
    QAction* createLayerSequenceSaveAction(){
        return Slib::createLambdaAction("LayerSequenceSave",[=]{
            vector < vector <Image>> images = scene->getImages();
            for(int frame = 0; frame < (int) images.size(); ++frame) {
                vector <Image> layers = images[frame];
                for(int layer = 0; layer < (int) layers.size(); ++layer) {
                    Image layerImg = layers[layer];
                    QString imgPath = createFileName(scene->getTitle(),frame,layer);
                    saveImage(layerImg,layerDir+imgPath);
                }
            }
        });
    }
    QAction* createPublishAction(){
        return Slib::createLambdaAction("Publish[white]",[=]{
            vector < vector <Image>> images = scene->getImages();
            for(int frame = 0; frame < (int) images.size(); ++frame) {
                Image displayImage = scene->getDisplayImage(frame,Qt::white);
                //
                QPainter publishPainter(&displayImage);
                QFont font;
                font.setBold(true);
                font.setPixelSize(24);
                publishPainter.setFont(font);
                publishPainter.setPen(QPen(Qt::gray));
                publishPainter.drawText(10,30,"# Project:"+scene->getTitle());
                //
                saveImage(displayImage,cacheDir+QString("%1-%2.png").arg(scene->getTitle()).arg(frame,3,10,QChar('0')));
            }
        });
    }
    void createStatusBar(){
        QStatusBar *statusbar = new QStatusBar(this);
        statusText = new QLabel();
        statusText->setAlignment(Qt::AlignRight);
        statusbar->addWidget(statusText,1);
        this->setStatusBar(statusbar);
    }
    bool saveFile(const QByteArray &fileFormat){
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save As"),
            cacheDir + "/"+scene->getTitle()+"."+fileFormat,
            tr("%1 Files (*.%2);;All Files (*)")
            .arg(QString::fromLatin1(fileFormat.toUpper()))
            .arg(QString::fromLatin1(fileFormat)));
        if (fileName.isEmpty()) {
            return false;
        } else {
            return scene->saveImage(fileName, fileFormat.constData());
        }
    }
    void createWidget();
};

#endif // MWINDOW_H
