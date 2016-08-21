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
    setObjectName("dBaseDesigner");
    setAcceptDrops(true);

    scene = new Scene(this);
    scene->setSceneRect(0,0,800,600);
    scene->setObjectName("dBaseDesignerScene");

    this->setScene(scene);
    setDragMode(QGraphicsView::RubberBandDrag);

    /*
    DesignerPushButtonItem *btn = new
    DesignerPushButtonItem(QString("Ok"),QRect(120,120,120,120),scene);

    DesignerRadioButtonItem *radio = new
    DesignerRadioButtonItem(QString("FooBar"),QRect(30,30,150,40),scene);

    DesignerLabelItem *label = new
    DesignerLabelItem(QString("Label1"),QRect(50,100,100,35),scene);

    DesignerGroupBoxItem *group1 = new
    DesignerGroupBoxItem(QString(" Group1: "),QRect(20,150,200,125),scene);
    */
}

CornerGrabber::CornerGrabber(QGraphicsItem *parent,  int corner) :
    QGraphicsItem(parent),
    mouseDownX(0),
    mouseDownY(0),
    _outterborderColor(Qt::black),
    _outterborderPen(),
    _width(6),
    _height(6),
    _corner(corner),
    _mouseButtonState(kMouseReleased)
{
    setParentItem(parent);

    _outterborderPen.setWidth(2);
    _outterborderPen.setColor(_outterborderColor);

   this->setAcceptHoverEvents(true);
}

void CornerGrabber::setMouseState(int s)
{
    _mouseButtonState = s;
}

int CornerGrabber::getMouseState()
{
    return _mouseButtonState;
}

int CornerGrabber::getCorner()
{
    return _corner;
}


// we have to implement the mouse events to keep the linker happy,
// but just set accepted to false since are not actually handling them

void CornerGrabber::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void CornerGrabber::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void CornerGrabber::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
}

void CornerGrabber::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(false);
}

void CornerGrabber::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(false);
}


// change the color on hover events to indicate to the use the object has
// been captured by the mouse

void CornerGrabber::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::black;
    this->update(0,0,_width,_height);
}

void CornerGrabber::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    _outterborderColor = Qt::red;
    this->update(0,0,_width,_height);
}

QRectF CornerGrabber::boundingRect() const
{
    return QRectF(0,0,_width,_height);
}


void CornerGrabber::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    // fill the box with solid color, use sharp corners

    _outterborderPen.setCapStyle(Qt::SquareCap);
    _outterborderPen.setStyle(Qt::SolidLine);
    painter->setPen(_outterborderPen);

    QPointF topLeft (0, 0);
    QPointF bottomRight ( _width, _height);

    QRectF rect (topLeft, bottomRight);

    QBrush brush (Qt::SolidPattern);
    brush.setColor (_outterborderColor);
    painter->fillRect(rect,brush);

}
