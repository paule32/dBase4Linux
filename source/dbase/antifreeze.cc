#include "source/includes/mainwindow.h"
#include "antifreeze.h"

MyAntiFreeze::MyAntiFreeze(QObject *parent, bool b)
    : QThread(parent)
    , Stop(b)
{
    steps = 1000;
}

void MyAntiFreeze::run()
{
    for (int i = 0; i <= steps; i++)
    {
        QMutex mutex;
        mutex.lock();   // prevent other threads changing stop value
        if (i > 0 && this->Stop) {
			if (w->isVisible()) 
			throw QString("plub");
			break;
		}
        mutex.unlock();

        emit valueChanged(i);
        this->msleep(11110);
    }
}
