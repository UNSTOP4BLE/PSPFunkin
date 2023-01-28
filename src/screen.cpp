#include "screen.h"
#include <cstdarg>
#include <cstring>  
#include <cstdio>
        
Screen *currentScreen;
char msg[256];

//error handler
void ErrMSG(const char *format, ...)
{
	va_list list;

    va_start(list, format);
    std::vsprintf(msg, format, list);
    va_end(list);

    setScreen(new ErrorScreen());
}

void ErrorScreen::draw(void) 
{
	PrintMSG(0, 0, "ERROR");
	PrintMSG(0, 20, msg);
}

void setScreen(Screen *scr)
{	
	if (currentScreen != NULL)
		delete currentScreen;
	currentScreen = scr;
	currentScreen->load();
}
