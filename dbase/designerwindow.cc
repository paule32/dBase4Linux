#include <QDialog>
#include <QWidget>

#include "designerwindow.h"

graphicsDesignWindow::graphicsDesignWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphicsDesignerWindow)
{
    ui->setupUi(this);
}

graphicsDesignWindow::~graphicsDesignWindow()
{

}
