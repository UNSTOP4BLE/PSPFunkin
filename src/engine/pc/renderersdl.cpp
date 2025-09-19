#include "renderersdl.hpp"
#include <cassert>
#include <ratio>

namespace GFX {

SDL_Renderer *g_sdlrenderer;

void SDLRenderer::init() {
    window = SDL_CreateWindow("PSPFunkin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    g_sdlrenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    surface = SDL_GetWindowSurface(window);
    SDL_SetRenderDrawBlendMode(g_sdlrenderer, SDL_BLENDMODE_BLEND);
    setClearCol(0x00FF00FF);
}

bool SDLRenderer::running() {
    SDL_Event event;
    int count = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT);
    return (count == 0); // if SDL_QUIT exists, stop
}

void SDLRenderer::beginFrame() {
    uint8_t r = (clearcol >> 24) & 0xFF;
    uint8_t g = (clearcol >> 16) & 0xFF;
    uint8_t b = (clearcol >> 8)  & 0xFF;
    uint8_t a = (clearcol)       & 0xFF;
    SDL_SetRenderDrawColor(g_sdlrenderer, r,g,b,a);
    SDL_RenderClear(g_sdlrenderer);
}

void SDLRenderer::endFrame() {
    SDL_RenderPresent(g_sdlrenderer);
}

void SDLRenderer::drawRect(RECT<int32_t> &rect, int z, uint32_t col) {
    uint8_t r = (col >> 24) & 0xFF;
    uint8_t g = (col >> 16) & 0xFF;
    uint8_t b = (col >> 8) & 0xFF;
    uint8_t a = col & 0xFF;

    SDL_SetRenderDrawColor(g_sdlrenderer, r, g, b, a);

    SDL_Rect sdlRect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(g_sdlrenderer, &sdlRect);
}

void SDLRenderer::drawTexRect(const Texture &tex, RECT<int32_t> &src, RECT<int32_t> &dst, int z, uint32_t col) {
    uint8_t r = (col >> 24) & 0xFF;
    uint8_t g = (col >> 16) & 0xFF;
    uint8_t b = (col >> 8) & 0xFF;
    uint8_t a = col & 0xFF;

    SDL_SetTextureColorMod(tex.tex, r, g, b);
    SDL_SetTextureAlphaMod(tex.tex, a);
    SDL_SetTextureBlendMode(tex.tex, SDL_BLENDMODE_BLEND);

    SDL_Rect _img = {src.x, src.y, src.w, src.h};
    SDL_Rect _disp = {dst.x, dst.y, dst.w, dst.h};
    assert(SDL_RenderCopy(g_sdlrenderer, tex.tex, &_img, &_disp) >= 0);

}

}