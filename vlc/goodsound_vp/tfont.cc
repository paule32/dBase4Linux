#include "tfont.h"

TFont::TFont()
{
    font = TTF_OpenFont(
         "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
       , 22);
}

TFont::TFont(std::string name, int size)
{
    font = TTF_OpenFont(name.c_str(), size);
}

TFont::~TFont()
{
    TTF_CloseFont(font);
}
