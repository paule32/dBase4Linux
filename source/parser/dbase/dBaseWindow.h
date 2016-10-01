#ifndef QDBASEWINDOW_H
#define QDBASEWINDOW_H

#include <QObject>
#include <QWidget>
#include <QEventLoop>
#include "includes/mainwindow.h"

namespace dBaseParser {
class dBaseWindow : public QWidget
{
    Q_OBJECT
public:
    explicit dBaseWindow(QWidget *parent = 0);
    void showModal();
signals:

public slots:
};
} // namespace
#endif
