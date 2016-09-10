#include "tpainter.h"
#include "tfont.h"
#include "tgraphicsitem.h"

TPainter::TPainter()
{
    objects.clear();
}

void TPainter::paint(SDL_Surface *surface)
{
    TGraphicsItem *item = objects.back();
    item->draw();  // <--
}

void TPainter::drawText(TFont font, int xpos, int ypos, std::string str)
{
    TGraphicsText *text = new TGraphicsText;
    text->xpos = xpos;
    text->ypos = ypos;
    text->str  = str;
    text->font = font;

    objects.push_back(text);
}
