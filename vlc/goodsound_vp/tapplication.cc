#include "tapplication.h"
#include "tfont.h"
#include "tpainter.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const char* WINDOW_TITLE = "SDL Start";

TApplication::TApplication() //QWidget *parent) : QMainWindow(parent)
{
    SDL_Init( SDL_INIT_VIDEO );
    TTF_Init();

    screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 0,
    SDL_HWSURFACE |
    SDL_DOUBLEBUF );
    SDL_WM_SetCaption( WINDOW_TITLE, 0 );
}

TApplication::~TApplication()
{
    TTF_Quit();
    SDL_Quit();
}

int TApplication::run()
{
    SDL_Event event;

    int gameRunning = 1;
    while (gameRunning)
    {
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameRunning = 0;
            }
        }
        //SDL_FillRect(app.screen, NULL, SDL_MapRGB(app.screen->format, 0, 0, 0));
        //SDL_BlitSurface(textSurface, NULL, app.screen, &textLocation);
        //SDL_Flip(screen);
    }
    return 0;
}
