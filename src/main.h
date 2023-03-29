#pragma once

#include "psp/audio_mixer.h"

class PSPFunkin
{
public:
    Audio::Mixer *audioMixer;
    char errstr[256];
    double deltatime;
};
extern PSPFunkin *app;

void ErrMSG(const char *filename, const char *function, int line, const char *expr);
#define ASSERTFUNC(expr) ((expr) ? ((void) 0) : ErrMSG(__FILE__, __func__, __LINE__, #expr))
#define ASSERTFUNCFAIL(msg) ErrMSG(__FILE__, __func__, __LINE__, msg)
