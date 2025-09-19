#pragma once
#include "../renderer.hpp"
#include <SDL2/SDL.h>

namespace GFX {
extern SDL_Renderer *g_sdlrenderer;

class SDLRenderer : public Renderer {
public:
	void init(void);
	bool running(void);

	void beginFrame(void);
	void endFrame(void);

	void drawRect(RECT<int32_t> &rect, int z, uint32_t col);
	void drawTexRect(const Texture &tex, RECT<int32_t> &src, RECT<int32_t> &dst, int z, uint32_t col);

private:
    SDL_Window *window;
    SDL_Surface *surface;
};
}
