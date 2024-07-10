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

union Color {
public:
    uint32_t value;
    struct {
        uint8_t r, g, b, a;
    };
};

struct Line {
public:
    Vec3<float> a, b;
    Color color;
};

struct LineVertex {
public:
    Color       color;
    Vec3<float> pos;
};
using Line = LineVertex[2];
    

struct Triangle {
public:
    Vec3<float> verts[3];
    Color color;
};

template<typename T> struct RECT {
public:
    T x, y, w, h;
};

struct TexturedTriangle {
public:
    Vec3<float> verts[3];
    Vec2<int> uv[3];
    Color colors[3];
};

struct Texture {
public:
    ~Texture(void);

    bool load(const char *path);
};

class Mat4 {
public:
    float m[4][4];

    void setIdentity(void);
    void setTranslation(Vec3<float> vec);
    void setRotation(Vec3<float> vec);
    void setScale(Vec3<float> vec);
    void multiply(const Mat4 &other, Mat4 &output) const;
    
};

class Renderer {
public:
    int screenwidth;
    int screenheight;

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

    void drawLines(const Line *prims, size_t count);

    void clear(Color color);
    void swapBuffers(void);
    void waitForVSync(void); 
};
