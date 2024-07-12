#pragma once

#include <string>   
#include <vector>   
#include <stdint.h>  
#include <pspgu.h>  
#include <pspdisplay.h>

#define BUF_WIDTH (512)

template<typename T> struct Vec3 {
public:
    T x, y, z;
};

template<typename T> struct Vec2 {
public:
    T x, y;
};

union Color {
public:
    uint32_t value;
    struct {
        uint8_t r, g, b, a;
    };
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

template<typename T> static inline T *allocateAligned(size_t length, size_t alignment) {
    auto ptr    = new T[length + alignment * 2 / sizeof(T)];
    auto offset = reinterpret_cast<uintptr_t>(ptr) % alignment;

    if (offset)
        ptr = reinterpret_cast<T *>(ptr + alignment - offset);

    return ptr;
}

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

    virtual void clear(Color color, int z) {(void)color; (void)z;}
    virtual void swapBuffers(void) {}
    virtual void waitForVSync(void) {}
};

class PSPRenderer : public Renderer {
private:
    std::vector<Mat4> _matrices;

    void _updateMatrix(int matrixType);

public:
    int screenwidth;
    int screenheight;
    
    void *list;
    void *buffer;
    
    PSPRenderer(int width, int height, int numBuffers);
    ~PSPRenderer(void);

    void drawLines(const Line *prims, size_t count);

    const Mat4 &getCurrentMatrix(void) const;
    int pushMatrix(const Mat4 &mat);
    int popMatrix(void);
    void resetMatrix(void);

    void clear(Color color, int z);
    void swapBuffers(void);
    void waitForVSync(void); 
};