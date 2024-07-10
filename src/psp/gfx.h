#pragma once

#include <string>   
#include <vector>   
#include <stdint.h>  
#include <pspgu.h>  

template<typename T> struct Vec3 {
public:
    T x, y, z;
};

template<typename T> struct Vec2 {
public:
    T x, y;
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

struct Texture {
public:
};

class Mat4 {
public:
    float m[4][4];

    void setIdentity(void);
    void setTranslation(Vec3<float> vec);
    void setRotation(Vec3<float> vec);
    void setScale(Vec3<float> vec);
    void multiply(const Mat4 &other, Mat4 &output);
};

class Renderer {
public:
    int screenwidth;
    int screenheight;

    Renderer(int width, int height, int numBuffers) {(void) width; (void) height; (void) numBuffers; }
    virtual ~Renderer(void) {}

    virtual void drawLines(const Line *prims, size_t count) {(void) prims; (void) count; }
    virtual void drawTriangles(const Triangle *prims, size_t count) {(void) prims; (void) count; }
    virtual void drawTexturedTriangles(const Texture &texture, const TexturedTriangle *prims, size_t count) {(void) texture; (void) prims; (void) count; }
 
    virtual int pushMatrix(const Mat4 &mat) {(void) mat; return 0;}
    virtual int popMatrix(void) {return 0;}
    virtual void resetMatrix(void) {}

    virtual void clear(Color color) {(void)color;}
    virtual void swapBuffers(void) {}
    virtual void waitForVSync(void) {}
};


class PSPRenderer : public Renderer {
public:
    int screenwidth;
    int screenheight;
    
    PSPRenderer(int width, int height, int numBuffers);
    ~PSPRenderer(void);

};
