#include "../main.h"
#include "font.h"
#include <cstdarg>
#include <string>  
#include <cstring>  
#include <cstdio>
#include "gfx.h"
#include "../main.h"

struct Font
{   // X pos  Y pos  width  height 
    int charX, charY, charW, charH;
};

struct Tex
{
    GFX::Texture* fonttex; 
    GFX::Texture* boldtex; 
};

Tex tex;

//include the fontmaps
#include "fonts/font_font.h"
#include "fonts/font_bold.h"

void FntInit()
{   
    tex.fonttex = GFX::loadTex("assets/font/font.png");
    tex.boldtex = GFX::loadTex("assets/font/boldfont.png");
}

static int Font_GetW(Font *font, const char *str)
{
    int c;
    int width = 0;
    while ((c = *str++) != '\0')
    {
        //Shift and validate character
        if ((c -= 0x20) >= 0x60)
            continue;
            
        //Add width
        width += font[c].charW;
    }
    return width;
}
    
bool boldAnim;
int animtimer;

static void PrintMSG(GFX::Texture *tex, Font *font, int x, int y, const char *str, bool bold, Align all)
{   
    //Draw string character by character
    int c;
    int xhold = x;
    
    switch (all)
    {
        case Center:
            x -= Font_GetW(font, str) >> 1;
            break;
        case Left:
            break;
        case Right:
            x -= Font_GetW(font, str);
            break;
    }


    while ((c = *str++) != '\0')
    {
        if (c == '\n')
        {
            x = xhold;
            if (all == Center)
                x -= Font_GetW(font, str) >> 1;
            if (bold)
                y += 38;
            else
                y += 11;
        }   
        //Shift and validate character
        if ((c -= 0x20) >= 0x60)
            continue;
        
        //Draw character
        GFX::RECT<int> font_Img = {font[c].charX, font[c].charY, font[c].charW, font[c].charH};
        if (bold)
        {
            if (!boldAnim)
                font_Img.y = font[c].charY;
            else 
                font_Img.y = font[c].charY + 138;
        }
        GFX::RECT<int> font_Disp = {x, y, font[c].charW, font[c].charH};
        GFX::drawTex<int>(tex, &font_Img, &font_Disp, false, 0, 255);
        x += font[c].charW - 1;
    }

}


void PrintFont(Align all, int x, int y, const char *format, ...)
{
    va_list list;
    
    char string[256] = "";

    va_start(list, format);
    std::vsprintf(string, format, list);
    va_end(list);
    
    PrintMSG(tex.fonttex, fontmap, x, y, string, false, all);
}


void Bold_Tick(void)
{
    animtimer += 1 + app->deltatime;

    if (animtimer > 3)
    {
        boldAnim = !boldAnim;
        animtimer = 0;
    }
}

void PrintBOLD(Align all, int x, int y, const char *format, ...)
{   
    va_list list;
    
    char string[256] = "";

    va_start(list, format);
    std::vsprintf(string, format, list);
    va_end(list);
    
    PrintMSG(tex.boldtex, boldmap, x, y, string, true, all);
}
