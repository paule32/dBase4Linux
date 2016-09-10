#ifndef TCOMPONENT_H
#define TCOMPONENT_H

#include <QObject>

namespace classes {
class TComponent : public QObject
{
    Q_OBJECT
public:
    explicit TComponent(QObject *parent = 0);

signals:

public slots:
};
}
#endif // TCOMPONENT_H
