#ifndef __FONT_H__
#define __FONT_H__

#include <cstring>
 
void FntInit(const char* path);
void FntFree();
void PrintMSG(int x, int y, const char *format, ...);

#endif
