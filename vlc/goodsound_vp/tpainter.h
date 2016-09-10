#ifndef TPAINTER_H
#define TPAINTER_H

#include <iostream>
#include <vector>

#include "tfont.h"
#include "tgraphicsitem.h"

class TPainter
{
public:
    TPainter();
    void begin();
    void end();
    void drawText(TFont font, int xpos, int ypos, std::string str);
protected:
    std::vector<TGraphicsItem*> objects;
};

#endif // TPAINTER_H
