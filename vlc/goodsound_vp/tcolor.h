#ifndef TCOLOR_H
#define TCOLOR_H

#include "SDL.h"

class TColor
{
public:
    TColor(int rc, int gc, int bc);
    TColor() { }
    SDL_Color sdl_color;
};

#endif // TCOLOR_H
