#include "screen.h"
#include <cstdarg>
#include <cstring>  
#include <cstdio>
#include "common.h"
        
GameScreens game;
Screen *currentScreen;

//error handler
void ErrMSG(const char *format, ...)
{
	va_list list;

    va_start(list, format);
    std::vsprintf(game.Error.msg, format, list);
    va_end(list);
}

void setScreen(Screen scr)
{
	currentScreen = &scr;
	currentScreen->init();
}

void TitleScreen::init(void) 
{

}

void TitleScreen::update(void) 
{
}