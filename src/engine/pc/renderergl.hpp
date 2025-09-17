#pragma once
#include "../renderer.hpp"
#include "glad.h"
#include <GLFW/glfw3.h>

namespace GFX {

class GLRenderer : public Renderer {
public:
	void init(void);
	bool running(void);

	void beginFrame(void);
	void endFrame(void);

	void drawRect(RECT<int32_t> rect, int z, uint32_t col);
	void drawTexRect(const Texture &tex, XY<int32_t> pos, int z, uint32_t col);

private:
	GLFWwindow* window;
};
}
