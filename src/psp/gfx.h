#pragma once

//#ifndef PSP
#include <SDL2/SDL.h>
//#endif

namespace GFX {

constexpr int SCREEN_WIDTH  = 480;
constexpr int SCREEN_HEIGHT = 272;

template<typename T> struct RECT {
public:
    T x, y, w, h;
};
/*
#ifdef PSP
struct Texture {
    int texW;  
    int texH;  
    int imgW;   
    int imgH;  
    int bit_depth;     
    int color_type;    
    int rowbytes;      
    uint32_t* texdata;  
};
#else
*/
typedef SDL_Texture Texture;
//#endif
void init(void);
void clear(int color);
void flip(void);
Texture *loadTex(const char *path);
void freeTex(Texture *tex);
template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, bool linear, float angle, int alpha);
template<typename T> void drawTexZoom(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, bool linear, float angle, int alpha, float zoom);
};