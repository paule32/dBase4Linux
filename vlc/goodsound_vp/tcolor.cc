#include "tcolor.h"

TColor::TColor(Uint8 rc, Uint8 gc, Uint8 bc)
{
    sdl_color.r = rc;
    sdl_color.g = gc;
    sdl_color.b = bc;
}

TColor::TColor()
{
}
