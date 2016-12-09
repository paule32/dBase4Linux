#include "source/includes/mainwindow.h"
#include "source/includes/helplistview.h"

HelpBrowser::HelpBrowser(QHelpEngine* helpEngine, QWidget* parent)
    : QTextBrowser(parent),
      helpEngine(helpEngine)
{
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name){
    if (name.scheme() == "qthelp")
    return QVariant(helpEngine->fileData(name)); else
    return QTextBrowser::loadResource(type, name);
}

bool HelpBrowser::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {   QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_Escape:
            {
                w->delHelpPanel();
                w->addEditPanel();
                return true;
            }
            break;
        }
    }
    return QTextBrowser::eventFilter(target,event);
}


HelpListView::HelpListView(const QVector<QVariant> &data, HelpListView *parent)
{
	parentItem = parent;
	itemData = data;
}

HelpListView::~HelpListView()
{
	qDeleteAll(childItems);
}

HelpListView *
HelpListView::child(int number)
{
	return childItems.value(number);
}

int
HelpListView::childCount() const
{
	return childItems.count();
}

int
HelpListView::childNumber() const
{
    if (parentItem)
    return parentItem->childItems.indexOf(const_cast<HelpListView*>(this));
    return 0;
}

int
HelpListView::columnCount() const
{
	return itemData.count();
}

QVariant
HelpListView::data(int column) const
{
	return itemData.value(column);
}

bool
HelpListView::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
	return false;

	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		HelpListView *item = new HelpListView(data,this);
		childItems.insert(position, item);
	}

	return true;
}

bool
HelpListView::insertColumns(int position, int columns)
{
	if (position < 0 || position > childItems.size())
	return false;

	for (int column = 0; column < columns; ++column) {
		itemData.insert(position, QVariant());
	}

	foreach (HelpListView *child, childItems)
	child->insertColumns(position, columns);

	return true;	
}

HelpListView *
HelpListView::parent()
{
	return parentItem;
}

bool
HelpListView::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
	return false;

	for (int row = 0; row < count; ++row)
	delete childItems.takeAt(position);

	return true;
}

bool
HelpListView::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > itemData.size())
	return false;

	for (int column = 0; column < columns; ++column)
	itemData.remove(position);

	foreach (HelpListView *child, childItems)
	child->removeColumns(position, columns);

	return true;
}

bool
HelpListView::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}


// HelpListViewModel

HelpListViewModel::HelpListViewModel(
	const QStringList &headers,
	const QString	  &data,
	QObject *parent
)	: QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    foreach (QString header, headers)
        rootData << header;

    rootItem = new HelpListView(rootData);
    setupModelData(data.split(QString("\n")), rootItem);
}

HelpListViewModel::~HelpListViewModel()
{
    delete rootItem;
}

int HelpListViewModel::columnCount(const QModelIndex & /* parent */) const
{
    return rootItem->columnCount();
}

QVariant
HelpListViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    HelpListView *item = getItem(index);

    return item->data(index.column());
}

Qt::ItemFlags
HelpListViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

HelpListView *
HelpListViewModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        HelpListView *item = static_cast<HelpListView *>(index.internalPointer());
        if (item)
        return item;
    }
    return rootItem;
}

QVariant
HelpListViewModel::headerData(
	int section,
	Qt::Orientation orientation,
    int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex
HelpListViewModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    HelpListView *parentItem = getItem(parent);

    HelpListView *childItem = parentItem->child(row);
    if (childItem)
    return createIndex(row, column, childItem);  else
    return QModelIndex();
}

bool
HelpListViewModel::insertColumns(
	int position,
	int columns,
	const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool
HelpListViewModel::insertRows(
	int position,
	int rows,
	const QModelIndex &parent)
{
    HelpListView *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex
HelpListViewModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    HelpListView *childItem  = getItem(index);
    HelpListView *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool
HelpListViewModel::removeColumns(
	int position,
	int columns,
	const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool
HelpListViewModel::removeRows(
	int position,
	int rows,
	const QModelIndex &parent)
{
    HelpListView *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int
HelpListViewModel::rowCount(const QModelIndex &parent) const
{
    HelpListView *parentItem = getItem(parent);

    return parentItem->childCount();
}

bool
HelpListViewModel::setData(
	const QModelIndex &index,
	const QVariant    &value,
	int   role)
{
    if (role != Qt::EditRole)
    return false;

    HelpListView *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool
HelpListViewModel::setHeaderData(
	int section,
	Qt::Orientation orientation,
    const QVariant &value,
	int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
    return false;

    bool result = rootItem->setData(section, value);

    if (result)
    emit headerDataChanged(orientation, section, section);

    return result;
}

void
HelpListViewModel::setupModelData(
	const QStringList &lines,
	HelpListView     *parent)
{
    QList<HelpListView*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            ++position;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QVector<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            HelpListView *parent = parents.last();
            parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());

            for (int column = 0; column < columnData.size(); ++column)
            parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
        }

        ++number;
    }
}
