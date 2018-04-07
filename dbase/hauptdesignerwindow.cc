#include "scene.h"
#include "hauptdesignerwindow.h"
#include "dragdroplistwidget.h"

HauptDesignerWindow::HauptDesignerWindow(QWidget *parent)
{
    QGraphicsView *view = nullptr;
    view = new QGraphicsView(parent);
    view->setObjectName("hauptViewer");
    view->setAcceptDrops(true);
    view->move(2,2);
    view->resize(width()-2,height()-2);
       
    scene = new Scene(this);
    scene->setObjectName("dBaseDesignerHauptWindowScene");
    scene->addText(view->objectName());

    view->setScene(scene);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->show();
}

HauptDesignerWindow::~HauptDesignerWindow()
{
    delete scene;
}

void HauptDesignerWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void HauptDesignerWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    qDebug() << "121212";
}

void HauptDesignerWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
    qDebug() << "asddasd";
}


void HauptDesignerWindow::dropEvent(QDropEvent *event)
{
    event->accept();
    qDebug() << "dfffdfdfdf";

    QObject * obj = event->source();
    DragDropListWidget * ptr = dynamic_cast<
    DragDropListWidget * >(obj);

    qDebug() << "elser";
}
