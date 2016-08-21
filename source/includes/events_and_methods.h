#ifndef EVENTS_AND_METHODS_H
#define EVENTS_AND_METHODS_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QTableWidget>

class events_and_methods : public QTableWidget
{
    Q_OBJECT
public:
    explicit events_and_methods(QWidget *parent = 0);
protected:
signals:

public slots:
};

#endif // EVENTS_AND_METHODS_H
