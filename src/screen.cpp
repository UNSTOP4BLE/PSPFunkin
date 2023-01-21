#include "screen.h"
#include <cstdarg>
#include <cstring>  
#include <cstdio>
#include "common.h"

//error handler
void ErrorScreen::update(const char *format, ...){
	char str[256] = "";
	va_list list;

    va_start(list, format);
    std::vsprintf(str, format, list);
    va_end(list);

	PrintMSG(0, 0, "ERROR");
	PrintMSG(0, 20, str);
}

void TitleScreen::init(void) 
{

}

void TitleScreen::update(void) 
{
}