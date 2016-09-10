#include "tpainter.h"
#include "tfont.h"
#include "tgraphicsitem.h"

TPainter::TPainter()
{
    objects.clear();
    objects.resize(2048);
}

void TPainter::begin()
{
    //textSurface = TTF_RenderText_Shaded(font, "This is my text.",
    //foregroundColor, backgroundColor);
}

void TPainter::end()
{
}

void TPainter::drawText(TFont font, int xpos, int ypos, std::string str)
{
    TGraphicsText *text = new
    TGraphicsText;
    SDL_Rect textLocation = { xpos, ypos, 0, 0 };
}
