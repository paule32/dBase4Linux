#include <iostream>
#include <string>

#include "SDL.h"
#include "tgraphicsitem.h"

void TGraphicsText::draw()
{
    this->surface = TTF_RenderText_Shaded(font.font, "sdasASD",
    fgColor, bgColor);
}
