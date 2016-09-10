#include "myprocess.h"
#include <QMessageBox>

MyProcess::MyProcess()
{
    QString        program = "./getwin.sh";
    QStringList    arguments;

    connect (this, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
    connect (this, SIGNAL(readyReadStandardError()),  this, SLOT(printError()));

    start(program, arguments);

    waitForFinished();
}

void MyProcess::printOutput()
{
    QByteArray byteArray = readAllStandardOutput();
    QStringList strLines = QString(byteArray).split("\n");

    QMessageBox::information(0,"StandardTest","strLines");
}

void MyProcess::printError()
{
    QByteArray byteArray = readAllStandardError();
    QStringList strLines = QString(byteArray).split("\n");

    QMessageBox::information(0,"ErrorTest","strLines");
}
