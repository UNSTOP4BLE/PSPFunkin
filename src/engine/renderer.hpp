#pragma once 

#include "psp/glib2d.hpp"
#include <stdint.h>

namespace GFX {
constexpr int SCREEN_WIDTH = 480;
constexpr int SCREEN_HEIGHT = 272;

template<typename T>
struct [[gnu::packed]] RECT {
    T x, y, w, h;

    RECT() = default;
    RECT(T _x, T _y, T _w, T _h) : x(_x), y(_y), w(_w), h(_h) {}
};

template<typename T>
struct [[gnu::packed]] XY {
    T x, y;

    XY() = default;
    XY(T _x, T _y) : x(_x), y(_y) {}
};
#ifdef PSP
struct Texture {
    g2dTexture *tex;
};
#else
struct Texture {
    uint32_t id;       
    int width;        
    int height;  
};
#endif
class Renderer {
public:
	virtual void init(void) {}
	virtual bool running(void) {return 0;}
	
	virtual void beginFrame(void) {}
	virtual void endFrame(void) {}

	virtual void drawRect(RECT<int32_t> rect, int z, uint32_t col) {}
	virtual void drawTexRect(const Texture &tex, XY<int32_t> pos, int z, int col) {}
};
}