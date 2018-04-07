#ifndef DESIGNERWINDOW_H
#define DESIGNERWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QEvent>

#include "scene.h"
#include ".uic/ui_designerwindow.h"

namespace Ui {
class graphicsDesignerWindow;
}

class graphicsDesignWindow : public QWidget
{
    Q_OBJECT
public:
    explicit graphicsDesignWindow(QWidget *parent = 0);
    explicit graphicsDesignWindow(QString image, QRect rect, Scene *s);
    
    ~graphicsDesignWindow();
    
public slots:
private:
    Ui::graphicsDesignerWindow * ui;
};

#endif // DESIGNERWINDOW_H
