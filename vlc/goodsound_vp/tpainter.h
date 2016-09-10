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
    void drawText(TFont font, int xpos, int ypos, std::string str);
protected:
    std::vector<TGraphicsItem*> objects;
private:
    void paint(SDL_Surface *surface);
};

#endif // TPAINTER_H
