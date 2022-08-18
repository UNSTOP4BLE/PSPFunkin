#include <cstdarg>
#include <cstring>  
#include <cstdio>
#include "glib2d.h"

static struct
{   // X pos  Y pos  width  height 
	int charX, charY, charW, charH;
} fontmap[0x60] = {
	{128, 30, 4, 9},  //space
	{80, 20, 3, 9}, //!
	{104, 30, 5, 9}, //"
	{88, 20, 7, 9}, //#
	{32, 30, 7, 9}, //$
	{96, 20, 7, 9}, //%
	{56, 30, 7, 9}, //&
	{112, 30, 3, 9}, //'
	{120, 20, 4, 9}, //(
	{128, 20, 4, 9}, //)
	{80, 30, 7, 9}, //*
	{168, 20, 5, 9}, //+
	{192, 20, 4, 9}, //,
	{168, 20, 5, 9}, //-
	{200, 20, 3, 9}, //.
	{8, 30, 7, 9}, // /
	{0, 20, 7, 9}, //0
	{8, 20, 7, 9}, //1
	{16, 20, 7, 9}, //2
	{24, 20, 7, 9}, //3
	{32, 20, 7, 9}, //4
	{40, 20, 7, 9}, //5
	{48, 20, 7, 9}, //6
	{56, 20, 7, 9}, //7
	{64, 20, 7, 9}, //8
	{72, 20, 7, 9}, //9
	{104, 20, 3, 9}, //:
	{120, 30, 3, 9}, //;
	{88, 30, 6, 9}, //<
	{184, 20, 6, 9}, //=
	{96, 30, 6, 9}, //>
	{0, 30, 7, 9}, //?
	{72, 30, 7, 9}, //@
	{0, 0, 7, 9}, //A
	{8, 0, 7, 9}, //B
	{16, 0, 7, 9}, //C
	{24, 0, 7, 9}, //D
	{32, 0, 7, 9}, //E
	{40, 0, 7, 9}, //F
	{48, 0, 7, 9}, //G
	{56, 0, 7, 9}, //H
	{64, 0, 5, 9}, //I
	{72, 0, 7, 9}, //J
	{80, 0, 7, 9}, //K
	{88, 0, 7, 9}, //L
	{96, 0, 7, 9}, //M
	{104, 0, 7, 9}, //N
	{112, 0, 7, 9}, //O
	{120, 0, 7, 9}, //P
	{128, 0, 7, 9}, //Q
	{136, 0, 7, 9}, //R
	{144, 0, 7, 9}, //S
	{152, 0, 7, 9}, //T
	{160, 0, 7, 9}, //U
	{168, 0, 7, 9}, //V
	{176, 0, 7, 9}, //W
	{184, 0, 7, 9}, //X
	{192, 0, 7, 9}, //Y
	{200, 0, 7, 9}, //Z
	{152, 20, 4, 9}, //[
	{16, 30, 7, 9}, // backslash
	{160, 20, 4, 9}, //]
	{64, 30, 5, 9}, //^
	{24, 30, 7, 9}, //_
	{40, 30, 4, 9}, //`
	{0, 10, 7, 9}, //a
	{8, 10, 7, 9}, //b
	{16, 10, 7, 9}, //c
	{24, 10, 7, 9}, //d
	{32, 10, 7, 9}, //e
	{40, 10, 6, 9}, //f
	{48, 10, 7, 9}, //g
	{56, 10, 7, 9}, //h
	{64, 10, 3, 9}, //i
	{72, 10, 5, 9}, //j
	{80, 10, 7, 9}, //k
	{88, 10, 3, 9}, //l
	{96, 10, 7, 9}, //m
	{104, 10, 7, 9}, //n
	{112, 10, 7, 9}, //o
	{120, 10, 7, 9}, //p
	{128, 10, 7, 9}, //q
	{136, 10, 7, 9}, //r
	{144, 10, 7, 9}, //s
	{152, 10, 5, 9}, //t
	{160, 10, 7, 9}, //u
	{168, 10, 7, 9}, //v
	{176, 10, 7, 9}, //w
	{184, 10, 7, 9}, //x
	{192, 10, 6, 9}, //y
	{200, 10, 7, 9}, //z
	{136, 20, 5, 9}, //{
	{112, 20, 3, 9}, //|
	{144, 20, 5, 9}, //}
	{48, 30, 7, 9}, //~
};

typedef struct 
{
    g2dTexture* fnttex; //1 tex, 2 font
} Tex;

Tex tex;

void FntInit(const char* path)
{	
	tex.fnttex = g2dTexLoad(path, G2D_SWIZZLE);
}

void FntFree()
{
	g2dTexFree(&tex.fnttex);
}

void PrintMSG(int x, int y, const char *format, ...)
{	
	//Draw string character by character
	int c;
	int xhold = x;

    va_list list;
    
	char string[256] = "";

    va_start(list, format);
    std::vsprintf(string, format, list);
    va_end(list);
            
    for (int i = 0; i < (int)strlen(string); i++)
	{        
		if ((c = string[i]) != '\0')
		{
			if (c == '\n')
			{
				x = xhold;
				y += 11;
			}
			//Shift and validate character
			if ((c -= 0x20) >= 0x60)
				continue;
			
			//Draw character
			Rect font_Img = {fontmap[c].charX, fontmap[c].charY, fontmap[c].charW, fontmap[c].charH};
			Rect font_Disp = {x, y, fontmap[c].charW, fontmap[c].charH};
			DrawG2DTex(tex.fnttex, &font_Img, &font_Disp, false, 0, 255);

			x += fontmap[c].charW - 1;
		}
	}
}
