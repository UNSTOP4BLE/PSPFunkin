#pragma once
#include "../main.h"
#ifdef PSP
#include <pspctrl.h>
#endif
#include <stdbool.h>

#ifndef PSP
#define PSP_CTRL_SELECT      SDLK_BACKSPACE
#define PSP_CTRL_START       SDLK_RETURN
#define PSP_CTRL_UP          SDLK_w
#define PSP_CTRL_RIGHT       SDLK_d
#define PSP_CTRL_DOWN        SDLK_s
#define PSP_CTRL_LEFT        SDLK_a
#define PSP_CTRL_LTRIGGER    SDLK_q
#define PSP_CTRL_RTRIGGER    SDLK_e
#define PSP_CTRL_TRIANGLE    SDLK_UP
#define PSP_CTRL_CIRCLE      SDLK_LEFT
#define PSP_CTRL_CROSS       SDLK_DOWN
#define PSP_CTRL_SQUARE      SDLK_RIGHT
#endif

extern bool         Pad_Init        (void);
extern void         Pad_Shutdown    (void);
extern void         Pad_Update      (void);
extern bool         Pad_Pressed     (const unsigned long Button);
extern bool         Pad_Held        (const unsigned long Button);
