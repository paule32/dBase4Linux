#ifndef TPAINTER_H
#define TPAINTER_H

#include <iostream>
#include <vector>

#include "tapplication.h"
#include "tfont.h"

class TApplication;
class TGraphicsItem;

class TPainter
{
public:
    TPainter();
    void drawImage(int xpos, int ypos, std::string name);
    void drawText(TFont *font, int xpos, int ypos, std::string str);
    void drawTicker(TFont *font, int xpos, int ypos, std::string name);
    void drawRectangle(Sint16 xpos, Sint16 ypos, Uint16 width, Uint16 height, TColor color);

    void paint();

    std::vector<TGraphicsItem*> objects;
};

#endif // TPAINTER_H
