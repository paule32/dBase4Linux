#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include "source/includes/mainwindow.h"
#include "pch.h"

class HelpBrowser : public QTextBrowser
{
public:
    explicit HelpBrowser(QHelpEngine* helpEngine, QWidget* parent = 0);
    QVariant loadResource (int type, const QUrl& name);
protected:
    bool eventFilter(QObject *target, QEvent *event);
private:
    QHelpEngine* helpEngine;
};

#endif // HELPBROWSER_H
