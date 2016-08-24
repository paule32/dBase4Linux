#include "qmymainwindow.h"

QMyMainWindow::QMyMainWindow(QWidget *parent) : QWidget(parent)
{
    resize(100,100);
    move  (100,100);
    setWindowTitle("Form1");
}

void QMyMainWindow::showModal()
{
    show();

    QEventLoop loop;
    QObject::connect(this, SIGNAL(destroyed()), & loop, SLOT(quit()));
    loop.exec();
}
