#ifndef _MAINCLASS_H_
#define _MAINCLASS_H_

#include <QObject>
#include <QAction>
#include <QtGui>
#include <QApplication>
#include <QWidgetAction>
#include <QMouseEvent>
#include <QToolBar>
#include "source/includes/mainwindow.h"

extern class MainWindow *w;

extern bool parseText(std::string,int);

class MyMainClass: public QObject
{     Q_OBJECT
public:
    MyMainClass();
public slots:
	void on_Exit(int ec, QProcess::ExitStatus);
};

class PushButtonActionWrapper: public QPushButton
{
public:
    PushButtonActionWrapper(const QIcon & icon, const QString & text, QToolBar *parent)
    {
        setIcon(icon);
        setObjectName(text);
    }
    void mousePressEvent(QMouseEvent *e) {
        QPushButton::mousePressEvent(e);
        /*if (text() == QString("Start..."))
        
        parseText(w->ui->editorWidget
                   ->document()
                   ->toPlainText().toStdString(),
        0);*/
    }
};

class PushButtonAction : public QWidgetAction
{
public:
    explicit PushButtonAction(const QIcon & icon, const QString & text, QToolBar *parent)
        : QWidgetAction(parent)
    {
        _parent = parent;
        setIcon(icon);
        setObjectName(text);
    }
protected:
    PushButtonActionWrapper * pb;
    QToolBar * _parent;

    virtual QWidget * createWidget(QWidget * parent) {
        pb = new PushButtonActionWrapper(icon(), objectName(), _parent);
        pb->setParent(_parent);
        pb->setText(objectName());
        return pb;
    }

    virtual void deleteWidget(QWidget * widget)  {
        delete pb;
        delete widget;
    }
};
#endif
