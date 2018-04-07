#include <QHeaderView>
#include "events_and_methods.h"

events_and_methods::events_and_methods(QWidget *parent)
    : QTableWidget(parent)
{
    if (columnCount() < 1)
        setColumnCount(1);

    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    setHorizontalHeaderItem(0, __qtablewidgetitem);

    if (rowCount() < 1)
        setRowCount(1);

    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();

    setVerticalHeaderItem(0, __qtablewidgetitem1);
    setGeometry(QRect(0, 0, 234, parent->height()));
    setStyleSheet(QStringLiteral("background-color: rgb(253, 229, 229);"));
    setShowGrid(true);

    setRowCount(4);

    horizontalHeader()->setCascadingSectionResizes(false);
    horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
}

