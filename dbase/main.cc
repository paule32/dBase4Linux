#include "source/includes/mainclass.h"
#include <cstdint>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

MyMainClass * mc = nullptr;

int main(int argc, char **argv)
{
    // ---------------------------------
    // IMPORTANT: set plugin path's ...
    // ---------------------------------  
    setenv("QT_QPA_PLATFORM_PLUGIN_PATH",
    "./plugins", 1);
    
    
	QApplication app(argc,argv);
	mc  = new MyMainClass;
	return app.exec();
}

