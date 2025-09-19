#pragma once
#include "../renderer.hpp"
#include <SDL2/SDL.h>

namespace GFX {
extern SDL_Renderer *g_sdlrenderer;

class SDLRenderer : public Renderer {
public:
	void init(void);
	bool running(void);

	inline void setClearCol(uint32_t col) {clearcol = col;}
	inline uint32_t getClearCol(void) {return clearcol;}

	void beginFrame(void);
	void endFrame(void);

	void drawRect(RECT<int32_t> &rect, int z, uint32_t col);
	void drawTexRect(const Texture &tex, RECT<int32_t> &src, RECT<int32_t> &dst, int z, uint32_t col);

private:
	uint32_t clearcol;
	SDL_Window *window;
    SDL_Surface *surface;
};
}
