#include "source/includes/mainwindow.h"

class MainWindow * w;
int main(int argc, char **argv)
{
    QApplication app(argc,argv);

    w = new MainWindow;
    w->resize(1280,1140);
    w->ui->editorWidget->setFocus();
    w->show();

    return app.exec();
}

