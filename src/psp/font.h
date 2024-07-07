#pragma once

#include "assetmanager.h"

enum Align
{   
    Center,
    Left,
    Right
};

enum Fonts
{   
    Font_Font,
    Font_Bold
};

struct Font
{   // X pos  Y pos  width  height 
    int charX, charY, charW, charH;
};

struct FontManager
{
private:
    const ImageAsset *tex; 
    float zoom;
    int alpha;
    Fonts font;
    Font *fontmap;
    int GetW(const char *str);
    int GetH();
    void PrintMSG(int x, int y, const char *str, Align all);
public:
    FontManager(Fonts dafont, const char *path);
    void Print(Align all, int x, int y, const char *format, ...);
    void setZoom(float _zoom);
    void setAlpha(int _alpha);
};