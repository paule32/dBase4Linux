#include "tgraphicsitem.h"

void TGraphicsText::draw()
{
    textSurface = TTF_RenderText_Shaded(font.font, "This is my text.",
    foregroundColor, backgroundColor);
}
