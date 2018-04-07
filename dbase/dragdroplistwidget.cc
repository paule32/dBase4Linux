#include <QObject>
#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "designeritem.h"
#include "editorgutter.h"
#include "dragdroplistwidget.h"
#include "hauptdesignerwindow.h"

DragDropListWidget::DragDropListWidget(QWidget * parent)
    : QTreeWidget(parent)
{
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setDefaultDropAction(Qt::TargetMoveAction);
    //setAlternatingRowColors(true);
    //setSelectionMode(QAbstractItemView::SingleSelection);
    //setSelectionBehavior(QAbstractItemView::SelectItems);
}

void DragDropListWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidgetItem *itemptr = itemAt(event->pos());
    if (!itemptr) return;

    QTreeWidget::mousePressEvent(event);

    QMimeData *mimeData = new QMimeData;
    mimeData->setText("actis: ");

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    Qt::DropAction dropAction =
    drag->exec(
        Qt::CopyAction |
        Qt::MoveAction ,
        Qt::CopyAction);

    QObject *tar = drag->target();
    if (tar == nullptr) {
        return;
    }
   
    tar->dumpObjectInfo();
   
    QModelIndex index = currentIndex();
    qDebug() << itemptr->toolTip(index.column());

    QList<
    QGraphicsView *> boards(findChildren<
    QGraphicsView *>("hauptViewer"));  QList<
    QGraphicsView *>::const_iterator it;
    
    qDebug() << boards.count();
    
    for (it = boards.constBegin(); it != boards.constEnd(); ++it) {
        qDebug() << (*it)->objectName();
    }
    
    //DesignerPushButtonItem *btn = new
    //DesignerPushButtonItem(QString("Ok"),QRect(20,20,100,100),(*it)->scene);

}
