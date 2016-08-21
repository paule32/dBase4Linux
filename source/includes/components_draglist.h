#ifndef COMPONENTS_DRAGLIST_H
#define COMPONENTS_DRAGLIST_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>

class QDragEnterEvent;
class QDropEvent;

class components_draglist : public QListWidget
{
public:
    explicit components_draglist(QWidget *parent = 0);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;

    bool ItemStandardFlag;

signals:
public slots:
};

class MyListWidgetItem: public QListWidgetItem
{
public:
    explicit MyListWidgetItem(QListWidget *p, QString name, QString icon_res, int mode);
};

#endif // COMPONENTS_DRAGLIST_H
