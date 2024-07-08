#include "font.h"
#include "../app.h"
#include <cstdarg>
#include <string>  
#include <cstring>  
#include <cstdio>

//include the fontmaps
#include "fonts/font_font.h"
#include "fonts/font_bold.h"

FontManager::FontManager(Fonts dafont, const char *path)
{   
    zoom = 1;
    alpha = 255;
    font = dafont;
    tex = app->assetmanager.get<ImageAsset>(path);
    switch (dafont)
    {
        case Font_Font:
            fontmap = font_fontmap;
            break;
        case Font_Bold:
            fontmap = font_boldmap;
            break;
    }
    PRINT_ALLOC();
}

int FontManager::GetW(const char *str)
{
    int c;
    int width = 0;
    while ((c = *str++) != '\0')
    {
        //Shift and validate character
        if ((c -= 0x20) >= 0x60)
            continue;
            
        //Add width
        width += fontmap[c].charW;
    }
    return width;
}

int FontManager::GetH()
{
    return fontmap[33].charH;
}
    
void FontManager::PrintMSG(int x, int y, const char *str, Align all)
{   
    //Draw string character by character
    int c;
    int xhold = x;
    
    switch (all)
    {
        case Center:
            x -= GetW(str) >> 1;
            break;
        case Left:
            break;
        case Right:
            x -= GetW(str);
            break;
    }
    x*=zoom;
    y*=zoom;

    while ((c = *str++) != '\0')
    {
        if (c == '\n')
        {
            x = xhold;
            if (all == Center)
                x -= GetW(str) >> 1;
            y += GetH();
            y += 3;
        }   
        //Shift and validate character
        if ((c -= 0x20) >= 0x60)
            continue;
        
        //Draw character
        GFX::RECT<int> font_Img = {fontmap[c].charX, fontmap[c].charY, fontmap[c].charW, fontmap[c].charH};
        if (font == Font_Bold)
        {
            if (!(static_cast<int>(app->timer.elapsedMS()/10) % 2))
                font_Img.y = fontmap[c].charY;
            else 
                font_Img.y = fontmap[c].charY + 69;
        }
        GFX::RECT<int> font_Disp = {x, y, fontmap[c].charW, fontmap[c].charH};
        GFX::drawTex<int>(&tex->image, &font_Img, &font_Disp, 0, alpha, zoom);
        x += fontmap[c].charW - 1;
    }

}

void FontManager::Print(Align all, int x, int y, const char *format, ...)
{
    va_list list;
    
    char string[1024] = "";

    va_start(list, format);
    std::vsprintf(string, format, list);
    va_end(list);
    
    PrintMSG(x, y, string, all);
    setZoom(1);
    setAlpha(255);
}

void FontManager::setZoom(float _zoom)
{
    zoom = _zoom;
}

void FontManager::setAlpha(int _alpha)
{
    alpha = _alpha;
}