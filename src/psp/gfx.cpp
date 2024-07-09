#include "gfx.h"
#include "../main.h"
#include "../app.h"
#include "../screen.h"
#include "hash.h"

PSPRenderer::PSPRenderer(int width, int height, int numBuffers) {
    app->screenwidth = width;
    app->screenheight = height;
    sceGuInit();
}

PSPRenderer::~PSPRenderer(void) {
    sceGuTerm(void);
}

void PSPRenderer::clear(Color color) {
    sceGuClearColor(color);
}

void PSPRenderer::swapBuffers(void) {
    //set buffer todo
    sceGuSwapBuffers();
}

void PSPRenderer:: waitForVSync(void) {
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
}

namespace GFX {

void init(void) {
}

void clear(int color) {
}

void flip(void) {
}


Texture::~Texture(void) {
    PRINT_FREE();
}

bool Texture::load(const char *path) {
//    std::string errmsg = "loading texture " + static_cast<std::string>(path) + " is NULL";
  //  ASSERTFUNC(handle, errmsg.c_str());
    PRINT_ALLOC();
    return true;
}

/*
typedef enum
{
    SDL_ScaleModeNearest, < nearest pixel sampling 
    SDL_ScaleModeLinear,  < linear filtering 
    SDL_ScaleModeBest     < anisotropic filtering 
} SDL_ScaleMode;
*/
void drawRect(GFX::RECT<int> *Disp, float zoom, uint8_t r, uint8_t g, uint8_t b) {

}

template<typename T> void drawTex(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom) {
    RECT<T> zoomDisp = {static_cast<T>((Disp->x * zoom) + (app->screenwidth * (1 - zoom) / 2)), static_cast<T>((Disp->y * zoom) + (app->screenheight * (1 - zoom) / 2)), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};

    if (Disp->x+Disp->w >= 0 && Disp->x <= app->screenwidth && Disp->y+Disp->h >= 0 && Disp->y <= app->screenheight)
    {
    }
}

template void drawTex<int>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, float angle, int alpha, float zoom);
template void drawTex<float>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, float angle, int alpha, float zoom);

template<typename T> void drawColTex(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b) {
    RECT<T> zoomDisp = {static_cast<T>((Disp->x * zoom) + (app->screenwidth * (1 - zoom) / 2)), static_cast<T>((Disp->y * zoom) + (app->screenheight * (1 - zoom) / 2)), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};

    if (Disp->x+Disp->w >= 0 && Disp->x <= app->screenwidth && Disp->y+Disp->h >= 0 && Disp->y <= app->screenheight)
    {
    }
}

template void drawColTex<int>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b);
template void drawColTex<float>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b);

};
