#pragma once

enum Align
{   
	Center,
	Left,
	Right
};

void FntInit(void);
void PrintFont(Align all, int x, int y, const char *format, ...);
void Bold_Tick(void);
void PrintBOLD(Align all, int x, int y, const char *format, ...);