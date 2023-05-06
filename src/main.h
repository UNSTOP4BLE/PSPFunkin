#pragma once

#include "psp/audio_mixer.h"
#include "chartparser.h"
#include "screen.h"

class PSPFunkin
{
public:
    float lerp(float position, float target, float speed);
    Audio::Mixer *audioMixer;
    ChartParser parser;
    double deltatime;
    g2dColor screenCol;
    Screen *currentScreen;
};
extern PSPFunkin *app;

void ErrMSG(const char *filename, const char *function, int line, const char *expr, const char *msg);
#define ASSERTFUNC(expr, msg) ((expr) ? ((void) 0) : ErrMSG(__FILE__, __func__, __LINE__, #expr, msg))