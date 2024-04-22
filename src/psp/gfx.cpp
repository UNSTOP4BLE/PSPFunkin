#include "gfx.h"
#include "../main.h"
#include "../app.h"
#include "../screen.h"
#include <SDL2/SDL_image.h>
#include "hash.h"

namespace GFX {

void init(void) {
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);
    app->window = SDL_CreateWindow("PSPFunkin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mode.w, mode.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
        printf("destroying %d\n", handle);
    }
    handle = nullptr;
}

bool Texture::load(const char *path) {
    if (handle)
        return true; // already loaded

    handle = IMG_LoadTexture(app->renderer, path);
    printf("loading %d\n", handle);
    std::string errmsg = "loading texture " + static_cast<std::string>(path) + " is NULL";
    ASSERTFUNC(handle, errmsg.c_str());
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

template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom) {
    //no need to check if the texture is null cus drawTex() does it
    RECT<T> zoomDisp = {static_cast<T>((Disp->x * zoom) + (app->screenwidth * (1 - zoom) / 2)), static_cast<T>((Disp->y * zoom) + (app->screenheight * (1 - zoom) / 2)), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};

    ASSERTFUNC(tex->getHandle(), "texture is NULL");

    if (Disp->x+Disp->w >= 0 && Disp->x <= app->screenwidth && Disp->y+Disp->h >= 0 && Disp->y <= app->screenheight)
    {
        SDL_Rect _img = {static_cast<int>(Img->x), static_cast<int>(Img->y), static_cast<int>(Img->w), static_cast<int>(Img->h)};
        SDL_Rect _disp = {static_cast<int>(zoomDisp.x), static_cast<int>(zoomDisp.y), static_cast<int>(zoomDisp.w), static_cast<int>(zoomDisp.h)};

        SDL_SetTextureAlphaMod(tex->getHandle(), alpha);
        SDL_SetTextureScaleMode(tex->getHandle(), SDL_ScaleModeBest);
        ASSERTFUNC(SDL_RenderCopyEx(app->renderer, tex->getHandle(), &_img, &_disp, static_cast<double>(angle), NULL, SDL_FLIP_NONE) >= 0, "failed to display sprite");
    }
}

template void drawTex<int>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, float angle, int alpha, float zoom);
template void drawTex<float>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, float angle, int alpha, float zoom);
};