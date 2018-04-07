#include "update_thread.h"
#include <QMutex>
#include <QDebug>

update_thread::update_thread(QObject *parent, bool b)
    : QThread(parent)
    , Stop(b)
{
    steps = 100;
}

void update_thread::run()
{
    for (int i = 0; i <= steps; i++)
    {
        QMutex mutex;
        mutex.lock();   // prevent other threads changing stop value
        if (this->Stop) break;
        mutex.unlock();

        emit valueChanged(i);
        this->msleep(20);
    }
}
