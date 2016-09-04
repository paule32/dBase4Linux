#ifndef QMYMAINWINDOW_H
#define QMYMAINWINDOW_H

#include <QObject>
#include <QWidget>
#include <QEventLoop>
#include "includes/mainwindow.h"

namespace dBaseParser {
class QMyMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QMyMainWindow(QWidget *parent = 0);
    void showModal();
signals:

public slots:
};
} // namespace
#endif // QMYMAINWINDOW_H
