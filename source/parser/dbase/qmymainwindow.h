#ifndef QMYMAINWINDOW_H
#define QMYMAINWINDOW_H

#include <QObject>
#include <QWidget>
#include <QEventLoop>

class QMyMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QMyMainWindow(QWidget *parent = 0);

signals:

public slots:
};

#endif // QMYMAINWINDOW_H
