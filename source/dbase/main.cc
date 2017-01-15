#include "source/includes/mainclass.h"

#include <iostream>
#include <csignal>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#include "singleapplication.h"
#include "mainprocess.h"

using namespace std;

jmp_buf return_to_ide;
int _debugger_present = -1;

#define debug_break()                    \
do {                                     \
    if (1 == _debugger_present) {        \
        _debugger_present = 1;           \
        signal(SIGTRAP, signalHandler);  \
        __asm__("int3");                 \
    }                                    \
} while(0)

void signalHandler(int signum)
{
	QMessageBox mb;

	mb.setText("A Error was detected!");
	mb.setInformativeText("Do you wan't to leave the application?");
	mb.setStandardButtons(
		QMessageBox::Save	|
		QMessageBox::Yes    |
		QMessageBox::Cancel);
	mb.setDefaultButton(QMessageBox::Save);

	int ret = mb.exec();
	switch (ret) {
		case QMessageBox::Yes:
			exit(-1);
		break;

		case QMessageBox::Save:
		break;

		case QMessageBox::Cancel:	 // reset
			_debugger_present = 2;
			longjmp(return_to_ide,1);
		break;

		default:
			_debugger_present = 2;
			longjmp(return_to_ide,1);
		break;
	};
}

MyMainClass * mc = nullptr;
QString		_app_name;

int main(int argc, char **argv)
{
	SingleApplication app(argc,argv);
	app.setStyle("WindowsXP");

	// ---------------------------
	// do basic start-up stuff ...
	// ---------------------------
/*	signal(SIGSEGV, signalHandler);
	signal(SIGTRAP, signalHandler);
	signal(SIGINT , signalHandler);
	signal(SIGILL , signalHandler);
	signal(SIGABRT, signalHandler);
	signal(SIGFPE , signalHandler);

	debug_break();*/

	// -------------------------------------------
	// this is the entry point part of our program
	// -------------------------------------------
	try {
		if (!app.isRunning()) {
//			setjmp(return_to_ide);
			if (mc == nullptr)
			mc = new MyMainClass;

			if (_debugger_present == 2) {
			//	_debugger_present = -1;
			//	_app_name = argv[0];
			//	MainProcess *process = new MainProcess;
			//	return 200;
			}	else {
				app.exec();
				return 0;
			}
		}
	} catch (...) { }

	return 0;
}

