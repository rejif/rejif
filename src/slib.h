#ifndef SLIB_H
#define SLIB_H

#include <QObject>
#include <QDebug>
#include <QApplication>
#include <QDateTime>
#include <QAction>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>

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
