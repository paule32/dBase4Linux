#ifndef TFONT_H
#define TFONT_H

#include <iostream>

#include "SDL.h"
#include "SDL_ttf.h"

#include "tcolor.h"

class TFont
{
public:
    TFont(std::string name, int size=22);
    TFont();
    ~TFont();

    TTF_Font *font;

    TColor fgColor;
    TColor bgColor;
};

#endif // TFONT_H
