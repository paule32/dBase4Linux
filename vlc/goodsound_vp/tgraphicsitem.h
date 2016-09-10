#ifndef TGRAPHICSITEM_H
#define TGRAPHICSITEM_H

#include <iostream>
#include <typeinfo>

#include "SDL.h"
#include "tfont.h"
#include "tcolor.h"

class TGraphicsItem
{
public:
    virtual void draw() = 0;
};

class TGraphicsText: public TGraphicsItem
{
public:
    void draw();

    TFont font;
    int xpos;
    int ypos;
    std::string str;

    SDL_Color foregroundColor;
    SDL_Color backgroundColor;

    SDL_Surface * textSurface;
};

#endif // TGRAPHICSITEM_H
