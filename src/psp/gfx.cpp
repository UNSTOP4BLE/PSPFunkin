#include "gfx.h"
#include "../main.h"
#include "../app.h"
#include "../screen.h"
#include <SDL2/SDL_image.h>
#define PCSCALE 1 // works by multiplying psp screen res by this number


#include "memory.h"

namespace GFX {

void init(void) {
    app->window = SDL_CreateWindow("PSPFunkin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * PCSCALE, SCREEN_HEIGHT * PCSCALE, SDL_WINDOW_SHOWN);
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    app->screenSurface = SDL_GetWindowSurface(app->window);
}

void clear(int color) {
    SDL_SetRenderDrawColor(app->renderer, ((color >> 16) & 0xFF) / 255.0, ((color >> 8) & 0xFF) / 255.0, ((color) & 0xFF) / 255.0, 255);
    SDL_RenderClear(app->renderer);
}

void flip(void) {
    SDL_RenderPresent(app->renderer);
}

Texture *loadTex(const char *path) {
    Texture *psptex = NULL;
    psptex = IMG_LoadTexture(app->renderer, path);
    ASSERTFUNC(psptex, "texture is NULL");
    return psptex;
}

void freeTex(Texture *tex) {
    SDL_DestroyTexture(tex);
}

template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha) {
    ASSERTFUNC(tex, "texture is NULL");

    if (Disp->x+Disp->w >= 0 && Disp->x <= SCREEN_WIDTH && Disp->y+Disp->h >= 0 && Disp->y <= SCREEN_HEIGHT)
    {
        SDL_Rect _img = {static_cast<int>(Img->x), static_cast<int>(Img->y), static_cast<int>(Img->w), static_cast<int>(Img->h)};
        SDL_Rect _disp = {static_cast<int>(Disp->x), static_cast<int>(Disp->y), static_cast<int>(Disp->w), static_cast<int>(Disp->h)};
        SDL_SetTextureAlphaMod(tex, alpha);
        ASSERTFUNC(SDL_RenderCopyEx(app->renderer, tex, &_img, &_disp, static_cast<double>(angle), NULL, SDL_FLIP_NONE) >= 0, "failed to display sprite");
    }
}

template void drawTex<int>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, float angle, int alpha);
template void drawTex<float>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, float angle, int alpha);

template<typename T> void drawTexZoom(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, float angle, int alpha, float zoom) {
    //no need to check if the texture is null cus drawTex() does it
    RECT<T> zoomDisp = {static_cast<T>((Disp->x - SCREEN_WIDTH/2) * zoom + SCREEN_WIDTH/2), static_cast<T>((Disp->y - SCREEN_HEIGHT/2) * zoom + SCREEN_HEIGHT/2), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};
    drawTex(tex, Img, &zoomDisp, angle, alpha);
}

template void drawTexZoom<int>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, float angle, int alpha, float zoom);
template void drawTexZoom<float>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, float angle, int alpha, float zoom);
};