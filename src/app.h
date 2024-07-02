#pragma once

#include "psp/audio_mixer.h"
#include "chartparser.h"
#include "timer.h"
#include "screen.h"
#include "psp/input.h"
#include "psp/font.h"


//#ifndef PSP
#include <SDL2/SDL.h>
//#endif

class PSPFunkin
{
public:
    std::vector<std::string> debugmessages; //debug terminal
    int screenwidth;
    int screenheight;
//#ifndef PSP
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *screenSurface;
    Input::Event event;
    FontManager *boldFont;
    FontManager *normalFont;
//#endif
    Timer timer;
    Audio::Mixer *audioMixer;
    ChartParser parser;
    double deltatime;
    int screenCol;
    Screen *currentScreen;
};

extern PSPFunkin *app;