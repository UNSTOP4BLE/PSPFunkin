#ifndef __FONT_H__
#define __FONT_H__

void FntInit(char *toload);
void FntFree();
void PrintMSG(const char *text, int x, int y, int font);

#endif
