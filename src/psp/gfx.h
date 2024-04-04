#pragma once

#include <SDL2/SDL.h>

namespace GFX {

constexpr int SCREEN_WIDTH  = 480;
constexpr int SCREEN_HEIGHT = 272;

template<typename T> struct RECT {
public:
    T x, y, w, h;
};

typedef SDL_Texture Texture;

void init(void);
void clear(int color);
void flip(void);
Texture *loadTex(const char *path);
void freeTex(Texture *tex);
template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha);
template<typename T> void drawTexZoom(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom);
};