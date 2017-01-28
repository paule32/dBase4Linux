#include "source/includes/mainclass.h"
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

MyMainClass * mc = nullptr;

int main(int argc, char **argv)
{
	QApplication app(argc,argv);
	mc  = new MyMainClass;
	return app.exec();
}

