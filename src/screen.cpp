#include "screen.h"
#include <cstdarg>
#include <cstring>  
#include <cstdio>

//error handler
#include "psp/font.h"
void ErrorScreen::update(const char *format, ...){
	char str[256] = "";
	va_list list;

    va_start(list, format);
    std::vsprintf(str, format, list);
    va_end(list);

	PrintMSG(0, 0, "ERROR");
	PrintMSG(0, 20, str);
}

//menu
void TitleScreen::init(void) 
{

}