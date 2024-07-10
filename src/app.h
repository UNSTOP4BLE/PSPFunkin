#pragma once

#include "psp/audio_mixer.h"
#include "chartparser.h"
#include "timer.h"
#include "screen.h"
#include "psp/input.h"
#include "psp/font.h"
#include "psp/assetmanager.h"

//#ifndef PSP
//#endif

class PSPFunkin
{
public:
    std::vector<std::string> debugmessages; //debug terminal

//#ifndef PSP
    Renderer *renderer;
    Input::Event event;
    FontManager *boldFont;
    FontManager *normalFont;
//#endif
    AssetManager assetmanager;
    Timer timer;
    Audio::Mixer *audioMixer;
    ChartParser parser;
    double deltatime;
    int screenCol;
    Screen *currentScreen;
};

extern PSPFunkin *app;