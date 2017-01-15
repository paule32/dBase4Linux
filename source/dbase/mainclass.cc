#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include "source/includes/mainclass.h"
#include "mainprocess.h"

       class MainWindow *w;
static class MyMainWindow *wm;

MyMainClass::MyMainClass() {
    w = new MainWindow;
    w->resize(1280,1140);
    w->ui->editorWidget->setFocus();
    w->show();

    /*
    QToolBar * toolBar= new QToolBar("Main Window Tool Bar");
    toolBar->insertAction(0, new PushButtonAction(QIcon(":/Refresh.gif"), "Refresh", toolBar));
    toolBar->addAction(QIcon(":/First.gif"), "FirstAction");
    toolBar->addAction(QIcon(":/Last.gif"), "LastAction");
    toolBar->insertAction(0, new PushButtonAction(QIcon(":/start.gif"), "Start...", toolBar));

    w->addToolBar(Qt::TopToolBarArea, toolBar);*/
    w->showMaximized();
}

void MyMainClass::on_Exit(int ec, QProcess::ExitStatus)
{
	//MainProcess *process = new MainProcess;
	//process->start();
}

