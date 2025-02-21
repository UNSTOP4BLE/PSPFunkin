//written by spicyjpeg
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <cstdint>

namespace Gfx {

/* Data types */

template<typename T> struct Vec2 {
public:
	T x, y;
};

template<typename T> struct Vec3 {
public:
	T x, y, z;
};

template<typename T> struct Vec4 {
public:
	T x, y, z, w;
};

// Rectangle with X/Y offsets of top left and bottom right corners
template<typename T> struct Rect {
public:
	T x1, y1, x2, y2;
};

// Rectangle with X/Y offsets of top left corner, width and height
template<typename T> struct RectWH {
public:
	T x, y, w, h;
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

union Color {
public:
	uint32_t value;
	struct {
		uint8_t r, g, b, a;
	};

    void setValue(uint32_t v) {
        value = v;
        r = (v >> 24) & 0xFF;
        g = (v >> 16) & 0xFF;
        b = (v >> 8) & 0xFF;
        a = v & 0xFF;
        printf("r%d g%d b%d a%d h%u\n", r,g,b,a,value);
    }

    void setValue(uint8_t rv,uint8_t gv,uint8_t bv,uint8_t av) {
		r=rv;
		g=gv;
		b=bv;
		a=av;
        value = (static_cast<uint32_t>(r) << 24) |
                (static_cast<uint32_t>(g) << 16) |
                (static_cast<uint32_t>(b) << 8) |
                static_cast<uint32_t>(a);
    	printf("r%d g%d b%d a%d h%u\n", r,g,b,a,value);
    }
};

// See sceGuDrawArray() documentation in
// https://github.com/pspdev/pspsdk/blob/master/src/gu/pspgu.h
struct Vertex {
public:
	// GU_COLOR_8888 | GU_VERTEX_32BITF
	Color       color;
	Vec3<float> pos;
};

struct TexturedVertex {
public:
	// GU_TEXTURE_16BIT | GU_COLOR_8888 | GU_VERTEX_32BITF
	Vec2<int16_t> uv;
	Color         color;
	Vec3<float>   pos;
};

using Point            = Vertex[1];
using Line             = Vertex[2];
using Triangle         = Vertex[3];
using TexturedTriangle = TexturedVertex[3];

// Quads are converted into two triangles by the drawing context.
using Quad         = Vertex[4];
using TexturedQuad = TexturedVertex[4];

/* Base renderer classes */

static constexpr size_t DISPLAY_LIST_LENGTH = 0x40000; // 256 KB

enum TextureFormat {
	TEXTURE_FMT_RGBA_32BPP = 0,
	TEXTURE_FMT_BC1        = 1,
	TEXTURE_FMT_BC2        = 2,
	TEXTURE_FMT_BC3        = 3
};

class Texture {
public:
	TextureFormat format;
	bool          bilinearFilter;

	int width, height;             // Size of actual image data
	int bufferWidth, bufferHeight; // Size of underlying texture buffer including padding

	virtual ~Texture(void) {}

	size_t getBufferSize(void) const;
	virtual void loadData(const char *data) {}
};

class Renderer {
public:
	int width, height;

	inline Renderer(int width, int height) : width(width), height(height) {}

	virtual ~Renderer(void) {}

	virtual void beginFrame(void) {}
	virtual void endFrame(void) {}
	virtual void swapBuffers(void) {}

	virtual void *allocatePrimData(size_t length) { return nullptr; }
	virtual void setMatrix(const Mat4 &mat) { (void) mat; }
	virtual void setTexture(const Texture &tex) { (void) tex; }

	virtual void clear(Color color, int z) { (void) color; (void) z; }
	virtual void drawPoints(const Point *prims, size_t count) { (void) prims; (void) count; }
	virtual void drawLines(const Line *prims, size_t count) { (void) prims; (void) count; }
	virtual void drawTriangles(const Triangle *prims, size_t count) { (void) prims; (void) count; }
	virtual void drawTexturedTriangles(const TexturedTriangle *prims, size_t count) { (void) prims; (void) count; }
};

/* Drawing context */

// How much space the drawing context should allocate at a time on the
// renderer's display list for primitive data.
static constexpr size_t DRAWING_CTX_ALLOC_LENGTH = 0x8000; // 32 KB

enum PrimitiveType {
	PRIM_TYPE_NONE               = 0,
	PRIM_TYPE_POINTS             = 1,
	PRIM_TYPE_LINES              = 2,
	PRIM_TYPE_TRIANGLES          = 3,
	PRIM_TYPE_TEXTURED_TRIANGLES = 4
};

class DrawingContext {
private:
	std::vector<Mat4> _matrices;

	PrimitiveType _currentPrimType;
	const Texture *_currentTexture;

	uint8_t *_primBuffer;
	size_t  _primBufferSize;

	void _flush(void);

	template<typename T> inline T *_addPrim(void) {
		if ((DRAWING_CTX_ALLOC_LENGTH - _primBufferSize) < sizeof(T))
			_flush();

		auto ptr         = reinterpret_cast<T *>(_primBuffer + _primBufferSize);
		_primBufferSize += sizeof(T);

		return ptr;
	}

	inline void _setPrimType(PrimitiveType type) {
		if (_currentPrimType != type) {
			_flush();
			_currentPrimType = type;
		}
	}
	inline void _setTexture(const Texture &tex) {
		if (_currentTexture != &tex) {
			_flush();
			_currentTexture = &tex;
		}
	}

public:
	Renderer *renderer;
	Color    backgroundColor;

	inline const Mat4 &getCurrentMatrix(void) const {
		return _matrices.back();
	}

	DrawingContext(void);

	void beginFrame(void);
	void endFrame(void);

	int pushMatrix(const Mat4 &mat);
	int popMatrix(void);
	void resetMatrix(void);

	void drawPoint(const Point &point);
	void drawLine(const Line &line);
	void drawTriangle(const Triangle &tri);
	void drawQuad(const Quad &quad);
	void drawTexturedTriangle(const Texture &tex, const TexturedTriangle &tri);
	void drawTexturedQuad(const Texture &tex, const TexturedQuad &quad);
};

}
