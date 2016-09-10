#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include "tapplication.h"
#include "tpainter.h"

int main(int argc, char **argv)
{
    TApplication app;

    TFont font("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
    font.fgColor = TColor(255,255,255);
    font.bgColor = TColor(0  ,  0,255);

    TPainter paint;
    paint.begin();
    paint.drawText(font,100,100,"This is my text.");
    paint.end();

    return app.run();
}
