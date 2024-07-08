#include "gfx.h"
#include "../main.h"
#include "../app.h"
#include "../screen.h"
#include <SDL2/SDL_image.h>
#include "hash.h"

namespace GFX {

void init(void) {
    app->screenwidth = 480;
    app->screenheight = 272;
    app->window = SDL_CreateWindow("PSPFunkin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app->screenwidth, app->screenheight, SDL_WINDOW_SHOWN);
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    app->screenSurface = SDL_GetWindowSurface(app->window);
}

void clear(int color) {
    SDL_GetWindowSize(app->window, &app->screenwidth, &app->screenheight);
    SDL_SetRenderDrawColor(app->renderer, ((color >> 16) & 0xFF) / 255.0, ((color >> 8) & 0xFF) / 255.0, ((color) & 0xFF) / 255.0, 255);
    SDL_RenderClear(app->renderer);
}

void flip(void) {
    SDL_RenderPresent(app->renderer);
}


Texture::~Texture(void) {
    if (handle) {
        SDL_DestroyTexture(handle);
    }
    handle = nullptr;
    PRINT_FREE();
}

//void Texture::setCol(uint8_t r, uint8_t g, uint8_t b) {
//    ASSERTFUNC(SDL_SetTextureColorMod(handle, r, g, b), SDL_GetError());
//} 

bool Texture::load(const char *path) {
    if (handle)
        return true; // already loaded

    handle = IMG_LoadTexture(app->renderer, path);
    std::string errmsg = "loading texture " + static_cast<std::string>(path) + " is NULL";
    ASSERTFUNC(handle, errmsg.c_str());
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
    SDL_Rect zoomDisp = {static_cast<int>((Disp->x * zoom) + (app->screenwidth * (1 - zoom) / 2)), static_cast<int>((Disp->y * zoom) + (app->screenheight * (1 - zoom) / 2)), static_cast<int>(Disp->w * zoom), static_cast<int>(Disp->h * zoom)};
    SDL_SetRenderDrawColor(app->renderer, r, g, b, 255);
    SDL_RenderFillRect(app->renderer, &zoomDisp);
}

template<typename T> void drawTex(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom) {
    RECT<T> zoomDisp = {static_cast<T>((Disp->x * zoom) + (app->screenwidth * (1 - zoom) / 2)), static_cast<T>((Disp->y * zoom) + (app->screenheight * (1 - zoom) / 2)), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};
//    zoomDisp.x -= 480;
 //   zoomDisp.y -= 272;

//    zoomDisp.w *= static_cast<float>((app->screenwidth/480));
  //  zoomDisp.h *= static_cast<float>((app->screenheight/272));
   // zoomDisp.x += app->screenwidth/2;
    //zoomDisp.y += app->screenheight/2;
    //zoomDisp.x *= (app->screenwidth/480);
    //zoomDisp.y *= (app->screenheight/272);

    ASSERTFUNC(tex->handle, "texture is NULL");

    if (Disp->x+Disp->w >= 0 && Disp->x <= app->screenwidth && Disp->y+Disp->h >= 0 && Disp->y <= app->screenheight)
    {
        SDL_Rect _img = {static_cast<int>(Img->x), static_cast<int>(Img->y), static_cast<int>(Img->w), static_cast<int>(Img->h)};
        SDL_Rect _disp = {static_cast<int>(zoomDisp.x), static_cast<int>(zoomDisp.y), static_cast<int>(zoomDisp.w), static_cast<int>(zoomDisp.h)};

        SDL_SetTextureAlphaMod(tex->handle, alpha);
        SDL_SetTextureScaleMode(tex->handle, SDL_ScaleModeBest);
        ASSERTFUNC(SDL_RenderCopyEx(app->renderer, tex->handle, &_img, &_disp, static_cast<double>(angle), NULL, SDL_FLIP_NONE) >= 0, SDL_GetError());
    }
}

template void drawTex<int>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, float angle, int alpha, float zoom);
template void drawTex<float>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, float angle, int alpha, float zoom);

template<typename T> void drawColTex(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b) {
    RECT<T> zoomDisp = {static_cast<T>((Disp->x * zoom) + (app->screenwidth * (1 - zoom) / 2)), static_cast<T>((Disp->y * zoom) + (app->screenheight * (1 - zoom) / 2)), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};
//    zoomDisp.x -= 480;
 //   zoomDisp.y -= 272;

//    zoomDisp.w *= static_cast<float>((app->screenwidth/480));
  //  zoomDisp.h *= static_cast<float>((app->screenheight/272));
   // zoomDisp.x += app->screenwidth/2;
    //zoomDisp.y += app->screenheight/2;
    //zoomDisp.x *= (app->screenwidth/480);
    //zoomDisp.y *= (app->screenheight/272);

    ASSERTFUNC(tex->handle, "texture is NULL");
    //tex->setCol(r, g, b);

    if (Disp->x+Disp->w >= 0 && Disp->x <= app->screenwidth && Disp->y+Disp->h >= 0 && Disp->y <= app->screenheight)
    {
        SDL_Rect _img = {static_cast<int>(Img->x), static_cast<int>(Img->y), static_cast<int>(Img->w), static_cast<int>(Img->h)};
        SDL_Rect _disp = {static_cast<int>(zoomDisp.x), static_cast<int>(zoomDisp.y), static_cast<int>(zoomDisp.w), static_cast<int>(zoomDisp.h)};
        SDL_SetTextureAlphaMod(tex->handle, alpha);
        SDL_SetTextureScaleMode(tex->handle, SDL_ScaleModeBest);
        ASSERTFUNC(SDL_RenderCopyEx(app->renderer, tex->handle, &_img, &_disp, static_cast<double>(angle), NULL, SDL_FLIP_NONE) >= 0, SDL_GetError());
//        tex->setCol(128, 128, 128);
    }
}

template void drawColTex<int>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b);
template void drawColTex<float>(const Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b);

};
