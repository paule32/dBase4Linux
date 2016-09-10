#ifndef TSURFACE_H
#define TSURFACE_H

#include <SDL.h>

class TMalFlaeche {
public:
    explicit TMalFlaeche();
private:
    SDL_Surface * surface_dst;
    SDL_Surface * surface_src;

    int xpos;
    int ypos;
};

class TBildAusschnitt: public TMalFlaeche {
public:
    TBildAusschnitt();

};

#endif // TSURFACE_H

