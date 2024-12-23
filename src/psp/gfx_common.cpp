//written by spicyjpeg
#include <cmath>
#include <stdint.h>
#include "gfx_common.h"
#include "../main.h"

namespace Gfx {

/* Data types */

void Mat4::setIdentity(void) {
    for (int i = 0; i < 4; ++i) {
        m[i][0] = (i == 0) ? 1.0f : 0.0f;
        m[i][1] = (i == 1) ? 1.0f : 0.0f;
        m[i][2] = (i == 2) ? 1.0f : 0.0f;
        m[i][3] = (i == 3) ? 1.0f : 0.0f;
    }
}

void Mat4::setTranslation(Vec3<float> vec) {
    setIdentity();
    m[0][3] = vec.x;
    m[1][3] = vec.y;
    m[2][3] = vec.z;
}

void Mat4::setRotation(Vec3<float> vec) {
    Mat4 rot, temp;

    float cosX = cosf(vec.x), sinX = sinf(vec.x);
    float cosY = cosf(vec.y), sinY = sinf(vec.y);
    float cosZ = cosf(vec.z), sinZ = sinf(vec.z);

    rot.setIdentity();
    rot.m[1][1] = cosX;
    rot.m[1][2] = -sinX;
    rot.m[2][1] = sinX;
    rot.m[2][2] = cosX;

    multiply(rot, temp);
    *this = temp;

    rot.setIdentity();
    rot.m[0][0] = cosY;
    rot.m[0][2] = sinY;
    rot.m[2][0] = -sinY;
    rot.m[2][2] = cosY;

    multiply(rot, temp);
    *this = temp;

    rot.setIdentity();
    rot.m[0][0] = cosZ;
    rot.m[0][1] = -sinZ;
    rot.m[1][0] = sinZ;
    rot.m[1][1] = cosZ;

    multiply(rot, temp);
    *this = temp;
}

void Mat4::setScale(Vec3<float> vec) {
    setIdentity();
    m[0][0] = vec.x;
    m[1][1] = vec.y;
    m[2][2] = vec.z;
}

void Mat4::multiply(const Mat4 &other, Mat4 &output) const {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            output.m[i][j] =
                m[i][0] * other.m[0][j] +
                m[i][1] * other.m[1][j] +
                m[i][2] * other.m[2][j] +
                m[i][3] * other.m[3][j];
    }
}

/* Base renderer classes */

size_t Texture::getBufferSize(void) const {
    switch (format) {
        case TEXTURE_FMT_RGBA_32BPP: // 1 pixel -> 32 bits
            return bufferWidth * bufferHeight * sizeof(Color);

        case TEXTURE_FMT_BC1: // 16 pixels -> 64 bits
            return bufferWidth * bufferHeight / 4;

        case TEXTURE_FMT_BC2: // 16 pixels -> 128 bits
        case TEXTURE_FMT_BC3:
            return bufferWidth * bufferHeight / 8;

        default:
            return 0;
    }
}

/* Drawing context */

DrawingContext::DrawingContext(void) : renderer(nullptr) {
    backgroundColor.value = 0xff000000;
    resetMatrix();
}

void DrawingContext::_flush(void) { 
//  renderer->setMatrix(_matrices.back()); crash

//  if (_currentTexture) crash
    //renderer->setTexture(*_currentTexture);

/*
    switch (_currentPrimType) { crash
        case PRIM_TYPE_NONE:
            break;

        case PRIM_TYPE_LINES:
            renderer->drawLines(
                reinterpret_cast<const Line *>(_primBuffer),
                _primBufferSize / sizeof(Line)
            );
            break;

        case PRIM_TYPE_TRIANGLES:
            renderer->drawTriangles(
                reinterpret_cast<const Triangle *>(_primBuffer),
                _primBufferSize / sizeof(Triangle)
            );
            break;

        case PRIM_TYPE_TEXTURED_TRIANGLES:
            renderer->drawTexturedTriangles(
                reinterpret_cast<const TexturedTriangle *>(_primBuffer),
                _primBufferSize / sizeof(TexturedTriangle)
            );
            break;
        default: break;
    } */
//  _primBuffer     = reinterpret_cast<uint8_t *>(
//      renderer->allocatePrimData(DRAWING_CTX_ALLOC_LENGTH) crash
//  );
    _primBufferSize = 0;
}

void DrawingContext::beginFrame(void) {
    ASSERTFUNC(renderer, "attempted to draw with no renderer");

    renderer->beginFrame();
    renderer->clear(backgroundColor, 0);

    resetMatrix();
    _currentPrimType = PRIM_TYPE_NONE;
    _currentTexture  = nullptr;

    _primBuffer     = reinterpret_cast<uint8_t *>(
        renderer->allocatePrimData(DRAWING_CTX_ALLOC_LENGTH)
    );
    _primBufferSize = 0;
}

void DrawingContext::endFrame(void) {
    _flush();
    renderer->endFrame();
    renderer->swapBuffers();
}

int DrawingContext::pushMatrix(const Mat4 &mat) {
    auto &newMatrix = _matrices.emplace_back();
    _matrices.back().multiply(mat, newMatrix);
    _flush();

    return _matrices.size();
}

int DrawingContext::popMatrix(void) {
    if (_matrices.size() > 1) {
        _matrices.pop_back();
        _flush();
    }

    return _matrices.size();
}

void DrawingContext::resetMatrix(void) {
    if (_matrices.size() != 1) {
        _matrices.clear();
        _matrices.emplace_back().setIdentity();
        _flush();
    }
}

void DrawingContext::drawPoint(const Point &point) {
    _setPrimType(PRIM_TYPE_POINTS);
    auto p = _addPrim<Point>();

    p[0]->color = point[0].color;
    p[0]->pos   = point[0].pos;
}

void DrawingContext::drawLine(const Line &line) {
    _setPrimType(PRIM_TYPE_LINES);
    auto p = _addPrim<Line>();

    for (int i = 0; i < 2; i++) {
        p[i]->color = line[i].color;
        p[i]->pos   = line[i].pos;
    }
}

void DrawingContext::drawTriangle(const Triangle &tri) {
    _setPrimType(PRIM_TYPE_TRIANGLES);
    auto p = _addPrim<Triangle>();

    for (int i = 0; i < 3; i++) {
        p[i]->color = tri[i].color;
        p[i]->pos   = tri[i].pos;
    }
}

void DrawingContext::drawQuad(const Quad &quad) {
    _setPrimType(PRIM_TYPE_TRIANGLES);
    auto p1 = _addPrim<Triangle>();
    auto p2 = _addPrim<Triangle>();

    p1[0]->color = quad[0].color;
    p1[0]->pos   = quad[0].pos;
    p1[1]->color = quad[1].color;
    p1[1]->pos   = quad[1].pos;
    p1[2]->color = quad[2].color;
    p1[2]->pos   = quad[2].pos;

    p2[0]->color = quad[1].color;
    p2[0]->pos   = quad[1].pos;
    p2[1]->color = quad[2].color;
    p2[1]->pos   = quad[2].pos;
    p2[2]->color = quad[3].color;
    p2[2]->pos   = quad[3].pos;
}

void DrawingContext::drawTexturedTriangle(const Texture &tex, const TexturedTriangle &tri) {
    _setPrimType(PRIM_TYPE_TEXTURED_TRIANGLES);
    _setTexture(tex);
    auto p = _addPrim<TexturedTriangle>();

    for (int i = 0; i < 3; i++) {
        p[i]->uv    = tri[i].uv;
        p[i]->color = tri[i].color;
        p[i]->pos   = tri[i].pos;
    }
}

void DrawingContext::drawTexturedQuad(const Texture &tex, const TexturedQuad &quad) {
    _setPrimType(PRIM_TYPE_TEXTURED_TRIANGLES);
    _setTexture(tex);
    auto p1 = _addPrim<TexturedTriangle>();
    auto p2 = _addPrim<TexturedTriangle>();

    p1[0]->uv    = quad[0].uv;
    p1[0]->color = quad[0].color;
    p1[0]->pos   = quad[0].pos;
    p1[1]->uv    = quad[1].uv;
    p1[1]->color = quad[1].color;
    p1[1]->pos   = quad[1].pos;
    p1[2]->uv    = quad[2].uv;
    p1[2]->color = quad[2].color;
    p1[2]->pos   = quad[2].pos;

    p2[0]->uv    = quad[1].uv;
    p2[0]->color = quad[1].color;
    p2[0]->pos   = quad[1].pos;
    p2[1]->uv    = quad[2].uv;
    p2[1]->color = quad[2].color;
    p2[1]->pos   = quad[2].pos;
    p2[2]->uv    = quad[3].uv;
    p2[2]->color = quad[3].color;
    p2[2]->pos   = quad[3].pos;
}

}
