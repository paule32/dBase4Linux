#include "source/includes/mainwindow.h"

HelpBrowser::HelpBrowser(QHelpEngine* helpEngine, QWidget* parent)
    : QTextBrowser(parent),
      helpEngine(helpEngine)
{
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name){
    if (name.scheme() == "qthelp")
    return QVariant(helpEngine->fileData(name)); else
    return QTextBrowser::loadResource(type, name);
}

bool HelpBrowser::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {   QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_Escape:
            {
                w->delHelpPanel();
                w->addEditPanel();
                return true;
            }
            break;
        }
    }
    return QTextBrowser::eventFilter(target,event);
}
