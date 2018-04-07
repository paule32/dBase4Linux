#ifndef SCENE_H
#define SCENE_H

#include "pch.h"
#include <QtWidgets/QGraphicsScene>

class Scene: public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(QObject *parent=0);
    int getGridSize() const { return this->gridSize;}

protected:
    void drawBackground(QPainter *painter, const QRectF &rect);
    
private:
    int gridSize;
};

#endif // SCENE_H
