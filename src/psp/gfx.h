#pragma once

#include <SDL2/SDL.h>
#include <string>   
#include <vector>   

namespace GFX {

template<typename T> struct RECT {
public:
    T x, y, w, h;
};

class Texture {
private:
public:
    SDL_Texture *handle;
    inline Texture(void)
    : handle(nullptr) {}
    inline SDL_Texture *getHandle(void) {
        return handle;
    }
//    void setCol(uint8_t r, uint8_t g, uint8_t b);
    ~Texture(void);
    bool load(const char *path);
};

void init(void);
void clear(int color);
void flip(void);
void drawRect(GFX::RECT<int> *Disp, float zoom, uint8_t r, uint8_t g, uint8_t b);
template<typename T> void drawTex(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom);
template<typename T> void drawColTex(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b);
};