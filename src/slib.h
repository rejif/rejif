#ifndef SLIB_H
#define SLIB_H

#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QDateTime>
#include <QAction>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDir>
#include <QLabel>
#include <QFontDatabase>
#include <QToolBar>
#include <QDockWidget>
class Slib : public QObject{
    Q_OBJECT
public:
    explicit Slib(QObject *parent = 0) :QObject(parent){
    }
    //	echo method
    static QString echo(const QString str){
        return "SlibEcho:"+str;
    }
    static QString getNow(QString format="yyyy/MM/dd-hh:mm:ss.zzz"){
        return QDateTime::currentDateTime().toString(format);
    }
    static QString mkdirp(QString dir){
        //dir = QDir::currentPath()+"/cache/"
        if(!QDir().exists(dir)) {
            if(!QDir().mkdir(dir)) {
                qDebug("%s %d: Error: QDir().mkdir(appdir)",__FILE__,__LINE__);
                QMessageBox::warning(0,"Error","Cache Directory could not be created.");
                exit(EXIT_FAILURE);
            }
        }
        return dir;
    }
    static QLabel* createLabel(QString str){
        QLabel *label = new QLabel("<div style=\"text-align:center;\">"+str+"</div>");
        label->setTextFormat(Qt::RichText);
        label->setTextInteractionFlags(Qt::TextBrowserInteraction);
        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        font.setPointSize(10);
        label->setFont(font);
        label->setOpenExternalLinks(true);
        label->setStyleSheet(
            "QLabel{"
                "color: rgba(255,255,255,100%);"
                "background-color: rgba(34,34,34,0.8);"
                "padding:3px 5px;"
            "}"
        );
        return label;
    }
    static QDockWidget* createDockWidget(const QString &title, QWidget *parent = Q_NULLPTR,Qt::WindowFlags flags = Qt::WindowFlags()){
        QDockWidget *dw = new QDockWidget(title,parent,flags);
        dw->setAllowedAreas(Qt::AllDockWidgetAreas);
        return dw;
    }
    static QToolBar* createToolBar(const QString &title, QWidget *parent = Q_NULLPTR){
        QToolBar *toolBar = new QToolBar(title,parent);
        toolBar->setAllowedAreas(Qt::AllToolBarAreas);
        return toolBar;
    }
    static QWidget* createBoxWidget(QBoxLayout* boxLayout){
        QWidget* w =new QWidget();
        w->setLayout(boxLayout);
        return w;
    }
    static QHBoxLayout* createHLayout(){
        QHBoxLayout *hl = new QHBoxLayout();
        hl->setSpacing(0);
        hl->setMargin(0);
        return hl;
    }
    static QVBoxLayout* createVLayout(){
        QVBoxLayout *vl = new QVBoxLayout();
        vl->setSpacing(0);
        vl->setMargin(0);
        return vl;
    }
    //QObjectConnectHelper
    static QMetaObject::Connection fowardConnect(const QObject *sender, const char *signal,const QObject *receiver, const char *member, Qt::ConnectionType = Qt::AutoConnection){
        return QObject::connect(sender,signal,receiver,member);
    }
    static QAction* createTestAction(){
        QAction* testAct = new QAction(tr("TestAction"));
        QObject::connect(testAct, &QAction::triggered,[=]{ qDebug()<<"AutoTestAction"; });
        return testAct;
    }
    static QPushButton* createTestButton(){
        QPushButton *testBtn = new QPushButton("TestButton");
        QAction* testAct = createTestAction();
        QObject::connect(testBtn,SIGNAL(clicked(bool)),testAct,SLOT(trigger()));
        return testBtn;
    }
    static QAction* createSlotAction(const QString actText,const QObject *receiver, const char *member, Qt::ConnectionType = Qt::AutoConnection){
        //createSlotAction(tr("SlotAction"),qApp, SLOT(quit()));
        QAction* act = new QAction(actText);
        connect(act, SIGNAL(triggered()),receiver,member);
        return act;
    }
    static QPushButton* createSlotActionButton(const QString btnText,const QObject *receiver, const char *member, Qt::ConnectionType = Qt::AutoConnection){
        return createActionButton(btnText,createSlotAction("SlotAction",receiver,member));
    }
    static QPushButton* createActionButton(const QString btnText,QAction* act){
        QPushButton *btn = new QPushButton(btnText);
        QObject::connect(btn,SIGNAL(clicked(bool)),act,SLOT(trigger()));
        return btn;
    }
    template<typename Functor>
    static QAction* createLambdaIconAction(QIcon icon,const QString actText,Functor f){
        QAction* act = new QAction(icon,actText);
        connect(act,&QAction::triggered,f);
        return act;
    }
    template<typename Functor>
    static QAction* createLambdaAction(const QString actText,Functor f){
        QAction* act = new QAction(actText);
        connect(act,&QAction::triggered,f);
        return act;
    }
    template<typename Functor>
    static QPushButton* createLambdaActionButton(QString btnText,Functor f){
        QPushButton *lambdaBtn = new QPushButton(btnText);
        QAction* lambdaAct = createLambdaAction("LambdaAction",f);//[=]{qDebug()<<"LambdaFunction";
        QObject::connect(
            lambdaBtn,SIGNAL(clicked(bool)),
            lambdaAct,SLOT(trigger())
        );
        return lambdaBtn;
    }
};

#endif // SLIB_H
