#include "dbasemainmenu.h"

dBaseMainMenu::dBaseMainMenu(class QWidget *parent)
    : QMenu(parent)
{     style_old = styleSheet();
}

void dBaseMainMenu::enterEvent(class QEvent * event)
{
    styleSheet().clear ();
    styleSheet().insert(0, QLatin1String(
    "background-color: rgb(159, 100, 100);\n"
    "color: yellow;"));
}

void dBaseMainMenu::leaveEvent(class QEvent * event)
{
    styleSheet().clear ();
    styleSheet().append(style_old);
}
