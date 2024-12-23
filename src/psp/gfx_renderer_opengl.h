//written by spicyjpeg and UNSTOP4BLE
#pragma once

#if _WIN32
#define ENABLE_OPENGL_RENDERER
#endif

#ifdef ENABLE_OPENGL_RENDERER

#include <cstddef>
#include <cstdint>
#include <vector>
#include "gfx_common.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace Gfx {

class OpenGLTexture : public Texture {
public:
    OpenGLTexture(TextureFormat _format, bool _bilinearFilter, int _width, int _height);
    ~OpenGLTexture(void);

    void loadData(const void *data);
};

class OpenGLRenderer : public Renderer {
private:
    SDL_Window* window;
    SDL_GLContext glContext;
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
public:
    OpenGLRenderer(int width, int height);
    ~OpenGLRenderer(void);

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
