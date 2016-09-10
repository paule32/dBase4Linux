#include "tpainter.h"
#include "tfont.h"
#include "tgraphicsitem.h"

TPainter::TPainter()
{
    objects.clear();
}

void TPainter::paint(TApplication &app)
{
    TGraphicsText *item = dynamic_cast<TGraphicsText*>(objects.back());
    item->draw();

    SDL_Rect textLocation = { item->xpos, item->ypos, 0, 0 };
    SDL_BlitSurface(item->surface, NULL, app.screen, &textLocation);
}

void TPainter::drawText(TFont &font, int xpos, int ypos, std::string str)
{
    TGraphicsText *text = new TGraphicsText;
    text->xpos = xpos;
    text->ypos = ypos;
    text->str  = str;
    text->font = font;

    objects.push_back(text);
}
