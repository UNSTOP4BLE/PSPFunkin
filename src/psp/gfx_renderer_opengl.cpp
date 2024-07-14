//written by spicyjpeg and UNSTOP4BLE
#ifdef ENABLE_OPENGL_RENDERER

#include "gfx_common.h"
#include "gfx_renderer_opengl.h"
#include "util.h"

namespace Gfx {

OpenGLTexture::OpenGLTexture(TextureFormat _format, bool _bilinearFilter, int _width, int _height) {
	// TODO
}

OpenGLTexture::~OpenGLTexture(void) {
	// TODO
}

void OpenGLTexture::loadData(const void *data) {
	// TODO
}

OpenGLRenderer::OpenGLRenderer(int width, int height) : Renderer(width, height) {
	// TODO
}

OpenGLRenderer::~OpenGLRenderer(void) {
	// TODO
}

void OpenGLRenderer::beginFrame(void) {
	// TODO
}

void OpenGLRenderer::endFrame(void) {
	// TODO
}

void OpenGLRenderer::swapBuffers(void) {
	// TODO
}

void *OpenGLRenderer::allocatePrimData(size_t length) {
	// TODO
	return nullptr;
}

void OpenGLRenderer::setMatrix(const Mat4 &mat) {
	// TODO
}

void OpenGLRenderer::setTexture(const Texture &tex) {
	// TODO
}

void OpenGLRenderer::clear(Color color, int z) {
	// TODO
}

void OpenGLRenderer::drawPoints(const Point *prims, size_t count) {
	// TODO
}

void OpenGLRenderer::drawLines(const Line *prims, size_t count) {
	// TODO
}

void OpenGLRenderer::drawTriangles(const Triangle *prims, size_t count) {
	// TODO
}

void OpenGLRenderer::drawTexturedTriangles(const TexturedTriangle *prims, size_t count) {
	// TODO
}

}

#endif
