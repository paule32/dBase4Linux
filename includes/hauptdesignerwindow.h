#ifndef HAUPTDESIGNERWINDOW_H
#define HAUPTDESIGNERWINDOW_H

#include <QtWidgets/QGraphicsView>
#include "pch.h"
#include "scene.h"

class HauptDesignerWindow : public QWidget
{
public:
    explicit HauptDesignerWindow(QWidget * parent = 0);
    ~HauptDesignerWindow();

protected:
    void dropEvent(QDropEvent *event);
    
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    
public:
    QGraphicsView *view;
    Scene* scene;
};

#endif // HAUPTDESIGNERWINDOW_H
