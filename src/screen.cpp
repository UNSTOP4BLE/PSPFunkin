#include "main.h"
#include "screen.h"
#include <cstdarg>
#include <cstring>  
#include <cstdio>
#include "psp/font.h"
        
Screen *currentScreen;
g2dColor screenCol;

void setScreen(Screen *scr)
{   
    if (currentScreen != NULL)
    {
        currentScreen->deload();
        delete currentScreen;
    }
    currentScreen = scr;
    currentScreen->load();
}

void setScreenCol(g2dColor color)
{
    screenCol = color;
}
