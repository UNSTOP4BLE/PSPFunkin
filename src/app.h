#pragma once

#include "psp/audio_mixer.h"
#include "chartparser.h"
#include "timer.h"
#include "screen.h"

//#ifndef PSP
#include <SDL2/SDL.h>
//#endif

class PSPFunkin
{
public:
//#ifndef PSP
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *screenSurface;
//#endif
    Timer timer;
    Audio::Mixer *audioMixer;
    ChartParser parser;
    double deltatime;
    int screenCol;
    Screen *currentScreen;
};

extern PSPFunkin *app;