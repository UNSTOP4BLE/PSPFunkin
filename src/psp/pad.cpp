#include "../main.h"
#include <string.h>
#ifdef PSP
#include <pspctrl.h>
#else
#include <SDL2/SDL.h>
#include <map>
#endif
#include <stdbool.h>

#include "pad.h"

struct Input
{
#ifdef PSP
    SceCtrlData   Input;
#endif
    unsigned long Pressed;
    unsigned long Held;
};

static Input Pad;

#ifndef PSP
std::map<int, bool> keyboard;
#endif

bool Pad_Init (void)
{
#ifdef PSP
    memset (&Pad, 0, sizeof(Input));

    sceCtrlSetSamplingCycle (0);

    sceCtrlSetSamplingMode (PSP_CTRL_MODE_ANALOG);

#endif
    return 1;
}

bool Pad_Pressed (int Button)
{
#ifdef PSP
    if (((Pad.Pressed & Button) != 0))  // Pressed is TRUE
    {
        Pad.Pressed &= ~Button;         // so set it to FALSE
        return 1;                       // and return TRUE
    }

    return 0;
#else
    return keyboard[Button];
#endif
}

bool Pad_Held (int Button)
{
#ifdef PSP
    return ((Pad.Held & Button) != 0);
#else
    return keyboard[Button];
#endif
}

static void AddInput (const unsigned long Button)
{
#ifdef PSP
    if ((Pad.Input.Buttons & Button) != 0)  // Button is down
    {
        if ((Pad.Held & Button) == 0)           // Held is not TRUE
        {
            Pad.Pressed |= Button;          // so set both Pressed
            Pad.Held    |= Button;          // and Held to TRUE
        }
    }
    else                                        // Button is up
    {
        Pad.Pressed &= ~Button;             // so set Presses
        Pad.Held    &= ~Button;             // and Held to FALSE
    }
#endif
}

void Pad_Update (void)
{
#ifdef PSP
    sceCtrlPeekBufferPositive (&Pad.Input, 1);
    AddInput (PSP_CTRL_SELECT);
    AddInput (PSP_CTRL_START);
    AddInput (PSP_CTRL_UP);
    AddInput (PSP_CTRL_RIGHT);
    AddInput (PSP_CTRL_DOWN);
    AddInput (PSP_CTRL_LEFT);
    AddInput (PSP_CTRL_LTRIGGER);
    AddInput (PSP_CTRL_RTRIGGER);
    AddInput (PSP_CTRL_TRIANGLE);
    AddInput (PSP_CTRL_CIRCLE);
    AddInput (PSP_CTRL_CROSS);
    AddInput (PSP_CTRL_SQUARE);
#else
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN: //key pressed
                keyboard[event.key.keysym.sym] = true;
                break;
            case SDL_KEYUP: //key released
                keyboard[event.key.keysym.sym] = false;
                break;
        }
    }
#endif
}
