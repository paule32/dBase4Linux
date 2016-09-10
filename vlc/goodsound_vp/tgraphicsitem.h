#ifndef TGRAPHICSITEM_H
#define TGRAPHICSITEM_H

#include <iostream>
#include <typeinfo>

#include "SDL.h"

#include "tapplication.h"
#include "tfont.h"
#include "tcolor.h"

class TApplication;
class TGraphicsItem
{
public:
    virtual void draw() = 0;
};

class TGraphicsText: public TGraphicsItem
{
public:
    TGraphicsText() { }
    void draw();

    TFont font;
    int xpos;
    int ypos;
    std::string str;

    SDL_Color fgColor;
    SDL_Color bgColor;

    SDL_Surface * surface;
};

#endif // TGRAPHICSITEM_H
