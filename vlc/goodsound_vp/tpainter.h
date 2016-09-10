#ifndef TPAINTER_H
#define TPAINTER_H

#include <iostream>
#include <vector>

#include "tfont.h"

class TApplication;
class TGraphicsItem;

class TPainter
{
public:
    TPainter();
    void drawText(TFont &font, int xpos, int ypos, std::string str);
    void paint(TApplication &app);
protected:
    std::vector<TGraphicsItem*> objects;
};

#endif // TPAINTER_H
