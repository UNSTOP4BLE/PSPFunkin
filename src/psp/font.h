#pragma once
#include "../main.h"

enum Align
{   
    Center,
    Left,
    Right
};

void FntInit(void);
void PrintFont(Align all, int x, int y, const char *format, ...);
void PrintFontZoom(Align all, int x, int y, float zoom, const char *format, ...);
void Bold_Tick(void);
void PrintBOLD(Align all, int x, int y, const char *format, ...);