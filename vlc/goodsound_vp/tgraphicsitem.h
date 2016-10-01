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
    SDL_Surface * surface;
    int zIndex;
    int itemType;

    Sint16 xpos;
    Sint16 ypos;

    Uint16 width;
    Uint16 height;

    SDL_Rect rect;
    TFont *font;

    std::string name;
};

class TGraphicsText: public TGraphicsItem
{
public:
    TGraphicsText() { }
    void draw();

    std::string str;
};

class TGraphicsRectangle: public TGraphicsItem
{
public:
    TGraphicsRectangle() { }
    void draw();

    TColor color;
};

class TGraphicsImage: public TGraphicsItem
{
public:
    TGraphicsImage(int x, int y, std::string name) { }
    void draw();
};

class TGraphicsTicker: public TGraphicsItem
{
public:
    TGraphicsTicker(int x, int y, std::string name) { }
    void draw();
};

#endif // TGRAPHICSITEM_H
