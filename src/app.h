#pragma once

#include "psp/audio_mixer.h"
#include "chartparser.h"
#include "timer.h"
#include "screen.h"
#include "psp/input.h"
#include "psp/font.h"
#include "psp/assetmanager.h"

class PSPFunkin
{
public:
    std::vector<std::string> debugmessages; //debug terminal

    Gfx::Renderer *renderer;
    Gfx::DrawingContext draw_ctx;

    Input::Event event;
    FontManager *boldFont;
    FontManager *normalFont;
    AssetManager assetmanager;
    Timer timer;
    Audio::Mixer *audioMixer;
    ChartParser parser;
    double deltatime;
    Gfx::Color screenCol;
    Screen *currentScreen;
};

extern PSPFunkin *app;