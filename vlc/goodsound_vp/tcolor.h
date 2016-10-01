#ifndef TCOLOR_H
#define TCOLOR_H

#include "SDL.h"

class TColor
{
public:
    TColor(Uint8 rc, Uint8 gc, Uint8 bc);
    TColor();
    SDL_Color sdl_color;
};

#endif // TCOLOR_H
