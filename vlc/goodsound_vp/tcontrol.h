#ifndef TCONTROL_H
#define TCONTROL_H

//#include <QObject>
//#include <QWidget>
#include "tpoint.h"

namespace classes {
typedef void * TReservedControlData;

class TControl //: public QWidget
{
    //Q_OBJECT
public:
    explicit TControl(/*QWidget *parent = 0*/);

protected:
    bool FAnchorMode;
    TPoint FAnchorOrigin;
    TPoint FAnchorRules;
    int FExplicitHeight;
    int FExplicitLeft;
    int FExplicitTop;
    int FExplicitWidth;
    TPoint FOriginalParentSize;
    TReservedControlData FReserved;
};
}

#endif // TCONTROL_H
