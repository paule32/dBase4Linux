#include "source/includes/mainclass.h"

int main(int argc, char **argv)
{
    QApplication app(argc,argv);

    MyMainClass *mc = new MyMainClass;
    return app.exec();
}

