//written by spicyjpeg
#pragma once

#ifdef PSP

#include <cstddef>
#include <cstdint>
#include <vector>
#include "gfx_common.h"

namespace Gfx {

class PSPTexture : public Texture {
public:
	// Textures are allocated in main RAM on the PSP.
	uint8_t *ptr;

	PSPTexture(TextureFormat _format, bool _bilinearFilter, int _width, int _height);
	~PSPTexture(void);

	void loadData(const void *data);
};

class PSPRenderer : public Renderer {
private:
	uint8_t *_displayLists[2];
	bool _currentDisplayList;

public:
	PSPRenderer(int width, int height);
	~PSPRenderer(void);

	void beginFrame(void);
	void endFrame(void);
	void swapBuffers(void);

	void *allocatePrimData(size_t length);
	void setMatrix(const Mat4 &mat);
	void setTexture(const Texture &tex);

	void clear(Color color, int z);
	void drawPoints(const Point *prims, size_t count);
	void drawLines(const Line *prims, size_t count);
	void drawTriangles(const Triangle *prims, size_t count);
	void drawTexturedTriangles(const TexturedTriangle *prims, size_t count);
};

}

#endif
