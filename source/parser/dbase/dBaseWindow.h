#ifndef QDBASEWINDOW_H
#define QDBASEWINDOW_H

#include <QObject>
#include <QWidget>
#include <QEventLoop>
#include "source/includes/mainwindow.h"

class dBaseWindow : public QWidget
{
    Q_OBJECT
public:
    explicit dBaseWindow(QWidget *parent = 0);
    void showModal();
signals:

public slots:
};

#endif
