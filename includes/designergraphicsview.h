#ifndef DESIGNERGRAPHICSVIEW_H
#define DESIGNERGRAPHICSVIEW_H

#include <QtWidgets/QGraphicsView>
#include "pch.h"
#include "designeritem.h"

class DesignerGraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    explicit DesignerGraphicsView(QWidget * parent = 0);
    Scene *getScene(void) { return scene; }
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
private:
    Scene* scene;
};

#endif // DESIGNERGRAPHICSVIEW_H
