
#include "mwindow.h"


void MWindow::createWidget(){
    QDockWidget *pw = Slib::createDockWidget(tr("Preview"));
    pw->setMaximumWidth(200);
    pw->setMaximumHeight(130);
    Preview* prev = new Preview();
    pw->setWidget(prev);
    addDockWidget(Qt::LeftDockWidgetArea,pw);
    pw->hide();

    QDockWidget *tw = Slib::createDockWidget(tr("Information"));
    tw->setMaximumWidth(200);
    tw->setMaximumHeight(200);
    Info* info = new Info();
    tw->setWidget(info);
    addDockWidget(Qt::LeftDockWidgetArea,tw);
    tw->hide();

    QDockWidget *dw = Slib::createDockWidget(tr("Dock"));
    dw->setMinimumWidth(200);
    QVBoxLayout *dwvl = Slib::createVLayout();
    dwvl->addStretch();
    dwvl->addWidget(Slib::createTestButton());
    dwvl->addWidget(
                Slib::createSlotActionButton(
                    "ScribbleAreaTestAction",
                    scene,SLOT(test())
                    )
                );
    dwvl->addStretch();
    dw->setWidget(Slib::createBoxWidget(dwvl));
    addDockWidget(Qt::LeftDockWidgetArea,dw);
    dw->hide();

    QToolBar *toolBar =Slib::createToolBar(tr("Tool"));
    toolBar->addAction(Slib::createLambdaIconAction(QIcon(":/rejif.png"),"ToolAction",[=]{
        qDebug()<<"ToolFunction";
    }));
    addToolBar(Qt::RightToolBarArea,toolBar);
    toolBar->hide();

    QToolBar *quickBar =Slib::createToolBar(tr("Quick"));
    quickBar->addAction(Slib::createLambdaIconAction(QIcon(":/rejif.png"),"ToolAction",[=]{
        qDebug()<<"QuickFunction";
    }));
    addToolBar(Qt::TopToolBarArea,quickBar);
    quickBar->hide();

    QToolBar *lambdaBar =Slib::createToolBar(tr("LambdaBar"));
    lambdaBar->addAction(Slib::createLambdaAction("InsertFrameAfter",[=]{
        scene->insertFrameAfter();
    }));
    addToolBar(Qt::TopToolBarArea,lambdaBar);

    QWidget *cw = new QWidget();
    setCentralWidget(cw);
    QHBoxLayout *hl = Slib::createHLayout();
    cw->setLayout(hl);
    QVBoxLayout *vl = Slib::createVLayout();
    hl->addLayout(vl);
    vl->addWidget(scene);
    Monitor* director=new Monitor();
    vl->addWidget(director);
}


