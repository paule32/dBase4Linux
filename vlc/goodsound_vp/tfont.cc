#include "tfont.h"

TFont::TFont(std::string name, int size)
{
    font = TTF_OpenFont(name.c_str(), size);
}

TFont::~TFont()
{
    TTF_CloseFont(font);
}
