#ifndef _ANTIFREEZE_
#define _ANTIFREEZE_

#include <QThread>
#include <QMutex>

class MyAntiFreeze: public QThread
{
	Q_OBJECT
public:
	explicit MyAntiFreeze(QObject *parent, bool active = false);
	void run();

	bool Stop;
	int  steps;

signals:
    void valueChanged(int);
};


#endif // _ANTIFREEZE_
