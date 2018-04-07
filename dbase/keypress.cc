#include "source/includes/mainwindow.h"

KeyPress::KeyPress(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void KeyPress::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Escape) {
       qApp->quit();
   }
}
