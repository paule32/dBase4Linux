#include "source/includes/mainwindow.h"

helpDocker::helpDocker(QWidget *parent)
    : QWidget(parent)
{
    installEventFilter(this);
}

bool helpDocker::onHelpClose()
{
    QMessageBox::information(this,"1111","222");

    w->delHelpPanel();
    w->addEditPanel();

    close();
    return true;
}

bool helpDocker::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_Escape:
            return onHelpClose();
            break;
        }
    }
    return QWidget::eventFilter(target,event);
}

void helpDocker::closeEvent(QCloseEvent *event)
{
    qDebug() << "AAA";
    if (w    ->helpWindow) w->delHelpPanel();
    //if (w->ui->editPage  ) w->addEditPanel();
    qDebug() << "BBB";
}
