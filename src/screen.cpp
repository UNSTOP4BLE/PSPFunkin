#include "main.h"
#include "screen.h"
#include <cstdarg>
#include <cstring>  
#include <cstdio>
#include "psp/font.h"
    
void setScreen(Screen *scr)
{   
    if (app->currentScreen != NULL)
        delete app->currentScreen;
    app->currentScreen = scr;
}

void setScreenCol(g2dColor color)
{
    app->screenCol = color;
}
