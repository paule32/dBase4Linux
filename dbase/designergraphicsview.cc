#include <QScreen>
#include "source/includes/mainwindow.h"

float scrollThreshold = 30;
void clamp(QPointF &value)
{
    if ((value.x() > scrollThreshold) || (value.x() < 0)) value.rx() = 0;
    else value.rx() = qAbs(value.x() - scrollThreshold);
    if ((value.y() > scrollThreshold) || (value.y() < 0)) value.ry() = 0;
    else value.ry() = qAbs(value.y() - scrollThreshold);
}

void DesignerGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F1)
    w->on_dockHelpOpen();
}


void DesignerGraphicsView::dropEvent(QDropEvent *event)
{
    event->accept();

    QObject * obj = event->source();
    components_draglist * ptr = dynamic_cast<
    components_draglist * >(obj);

    if (ptr != nullptr) {
        if (ptr->currentItem()->text() == "Addition") {
            DesignerSymbol_Addition *item1 = new
            DesignerSymbol_Addition(
            QString(":/images/addierer.png"),
            QRect(
                event->pos().x()- 100,
                event->pos().y(), 100, 100),
            scene);
            return;
        }
    }
}

void DesignerGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

DesignerGraphicsView::DesignerGraphicsView(QWidget * parent)
    : QGraphicsView(parent)
{
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setObjectName("dBaseDesigner");
    setAcceptDrops(true);

    int scr_width  = qApp->screens().at(0)->availableSize().width();
    int scr_heigth = qApp->screens().at(0)->availableSize().height();
    
    scene = new Scene(this);
    scene->setSceneRect(0,0, scr_width,scr_heigth);
    scene->setObjectName("dBaseDesignerScene");
    
    this->setScene(scene);
    setDragMode(QGraphicsView::RubberBandDrag);

    verticalScrollBar  ()->setSliderPosition(1);
    horizontalScrollBar()->setSliderPosition(1);


//    DesignerWindow *dwin = new
//    DesignerWindow(QString("MainWindow"),QRect(0,0,300,300),scene);
    
    DesignerHLayout *hlay = new
    DesignerHLayout(QRect(10,10,100,100),scene);
    
    
    DesignerPushButtonItem *btn = new
    DesignerPushButtonItem(QString("Ok"),QRect(10,20,80,80),scene);
/*
    DesignerRadioButtonItem *radio = new
    DesignerRadioButtonItem(QString("FooBar"),QRect(30,30,150,40),scene);

    DesignerLabelItem *label = new
    DesignerLabelItem(QString("Label1"),QRect(50,100,100,35),scene);

    DesignerGroupBoxItem *group1 = new
    DesignerGroupBoxItem(QString(" Group1: "),QRect(20,150,200,125),scene);
    */
}
