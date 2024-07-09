#pragma once

#include <string>   
#include <vector>   
#include <stdint.h>  
#include <pspgu.h>  

template<typename T> struct Vec3 {
public:
    T x, y, z;
};

using Color = uint32_t; // RGBA color

struct Line {
public:
    Vec3<float> a, b;
    Color color;
};

struct Triangle {
public:
    Vec3<float> verts[3];
    Color color;
};

struct TexturedTriangle {
public:
    Vec3<float> verts[3];
    Vec2<int> uv[3];
    Color colors[3];
};

class Renderer {
public:
    Renderer(int width, int height, int numBuffers) {}
    virtual ~Renderer(void) {}

    virtual void drawLines(const Line *prims, size_t count) {}
    virtual void drawTriangles(const Triangle *prims, size_t count) {}
    virtual void drawTexturedTriangles(const Texture &texture, const TexturedTriangle *prims, size_t count) {}
    virtual void drawSprites(const Texture &texture, const Sprite *prims, size_t count) {}

    virtual int pushMatrix(const Matrix &mat) {}
    virtual int popMatrix(void) {}
    virtual void resetMatrix(void) {}

    virtual void clear(Color color) {}
    virtual void swapBuffers(void) {}
    virtual void waitForVSync(void) {}
};


class PSPRenderer : public Renderer {
public:
    PSPRenderer(int width, int height, int numBuffers);
    ~PSPRenderer(void);

};
