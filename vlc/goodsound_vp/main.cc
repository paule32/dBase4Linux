#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL.h>
#include <vlc/vlc.h>

#include "tapplication.h"
#include "tpainter.h"

int main(int argc, char **argv)
{
    // GUI
    TApplication app;

    TFont font("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
    font.fgColor = TColor(255,255,255);
    font.bgColor = TColor(0  ,  0,255);

    TPainter paint;

    paint.drawImage(180,-5,"./data/logo.bmp");
    paint.drawImage(10 ,10,"./data/amp.bmp");
    paint.drawImage(900,10,"./data/amp.bmp");

    paint.drawImage(10,250,"./data/chat.bmp");
    paint.drawImage(10,320,"./data/settings.bmp");

    // chat box
    paint.drawRectangle(90,160,820,350,TColor(0,0,0));
    paint.drawRectangle(90,520,820,200,TColor(255,255,255));

    // chat text
    paint.drawRectangle(90,730,820,50 ,TColor(255,255,255));
    paint.drawImage(920,730,"./data/send.bmp");

    paint.drawRectangle(50,90,900,5,TColor(105,250,155));
    paint.drawTicker(&font,100,100,std::string("This is my text."));

    return app.run(paint);
}
