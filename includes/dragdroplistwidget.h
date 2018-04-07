#ifndef DRAGDROPTREEWIDGET_H
#define DRAGDROPTREEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class DragDropListWidget : public QTreeWidget
{
public:
    explicit DragDropListWidget(QWidget * parent = 0);
    
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // DRAGDROPTREEWIDGET_H
