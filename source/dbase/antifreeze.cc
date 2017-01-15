#include "antifreeze.h"
// -----------------------------------------------
// this ns check all 5 seconds, if the application
// produce inf memory leaks or simply freeze.
// Is so, then try to free memory and exit app ...
// -----------------------------------------------
namespace antifreeze
{
	using namespace std;

	int appSize = 0, oldSize = 0;
	int memDyna = 0, oldDmem = 0;

	// ------------------------------
	// start of monitoring memory ...
	// ------------------------------
	void MyTimer::MyTimerSlot()
	{
		qDebug() << "Memory thread initialized...";

		if (oldSize > appSize+10) {
		}

		ifstream buffer("/proc/self/statm");
		buffer  >> appSize
				>> memDyna;

		long page_size_kb = sysconf(_SC_PAGESIZE) / 1024;
		double rss = memDyna * page_size_kb;

		qDebug() << "Mem: " << rss;
	}

	// -----------------------------
	// simple timer function:
	// set interval, connect to proc
	// -----------------------------
	MyTimer::MyTimer()
	{
#ifdef TIMERS_TESTERS
		timer = new QTimer(this);
		connect(timer,
			SIGNAL(timeout()),this,
			SLOT(MyTimerSlot()));

		timer->start(5000);  // check memory/freeze all 5 seconds
#endif
	}

	// simple doofy
	void init() { /*MyTimer *ltimer = new MyTimer; qDebug() << "gesetinhs"; */ }

}  // namespace antifreeze

