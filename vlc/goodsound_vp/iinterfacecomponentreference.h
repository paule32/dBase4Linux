#ifndef IINTERFACECOMPONENTREFERENCE_H
#define IINTERFACECOMPONENTREFERENCE_H

#include "tcomponent.h"

namespace classes {
class IInterfaceComponentReference
{
public:
    virtual TComponent * GetComponent();
    virtual ~IInterfaceComponentReference() { }
};
}
#endif // IINTERFACECOMPONENTREFERENCE_H
