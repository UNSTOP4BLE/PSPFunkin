#include "file.hpp"
#include <cassert>
#ifndef PSP
#include "pc/renderersdl.hpp"
#endif

namespace FS {
GFX::Texture loadTexFile(const std::string& path) {
#ifdef PSP
    
#else
    GFX::Texture tex;
    SDL_Surface* surface = IMG_Load(path.c_str());
    assert(surface);

    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    tex.tex = SDL_CreateTextureFromSurface(GFX::g_sdlrenderer, converted);
    SDL_FreeSurface(converted);

    assert(tex.tex);
    SDL_SetTextureBlendMode(tex.tex, SDL_BLENDMODE_BLEND);

    return tex;

#endif
    //default
    return GFX::Texture();
}
}