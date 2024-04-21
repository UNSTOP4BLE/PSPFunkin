#pragma once

#include <SDL2/SDL.h>
#include <string>   
#include <vector>   

namespace GFX {

#if defined(__vita__)
constexpr int SCREEN_WIDTH  = 960;
constexpr int SCREEN_HEIGHT = 544;
#elif defined(PSP)
constexpr int SCREEN_WIDTH  = 480;
constexpr int SCREEN_HEIGHT = 272;
#else
constexpr int SCREEN_WIDTH  = 1280;
constexpr int SCREEN_HEIGHT = 720;
#endif
constexpr float SCALEFACTOR_X = SCREEN_WIDTH/480;
constexpr float SCALEFACTOR_Y = SCREEN_HEIGHT/272;

template<typename T> struct RECT {
public:
    T x, y, w, h;
};

class Texture {
private:
    SDL_Texture *handle;
public:
    inline Texture(void)
    : handle(nullptr) {}
    inline SDL_Texture *getHandle(void) {
        return handle;
    }

    ~Texture(void);
    bool load(const char *path);
};

void init(void);
void clear(int color);
void flip(void);
template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom);
};