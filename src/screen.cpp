#include "main.h"
#include "app.h"
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

void setScreenCol(uint32_t color)
{
    app->screenCol.value = color;
}
