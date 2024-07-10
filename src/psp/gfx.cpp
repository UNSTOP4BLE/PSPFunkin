#include "gfx.h"
#include "../main.h"
#include "../app.h"


Texture::~Texture(void) {
    PRINT_FREE();
}

bool Texture::load(const char *path) {
//    std::string errmsg = "loading texture " + static_cast<std::string>(path) + " is NULL";
  //  ASSERTFUNC(handle, errmsg.c_str());
    PRINT_ALLOC();
    return true;
}

void Mat4::setIdentity(void) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
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

    float cosX = cos(vec.x), sinX = sin(vec.x);
    float cosY = cos(vec.y), sinY = sin(vec.y);
    float cosZ = cos(vec.z), sinZ = sin(vec.z);

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
        for (int j = 0; j < 4; j++) {
            output.m[i][j] = 0;
                for (int k = 0; k < 4; k++)
                    output.m[i][j] += m[i][k] * other.m[k][j]; 
        }
    }
}

PSPRenderer::PSPRenderer(int width, int height, int numBuffers) {
    screenwidth = width;
    screenheight = height;
    sceGuInit();
}

PSPRenderer::~PSPRenderer(void) {
    sceGuTerm();
}

void PSPRenderer::drawLines(const Line *prims, size_t count) {
    int flags = 0
    | GU_COLOR_8888    // 24bpp vertex colors
    | GU_VERTEX_32BITF // float vertices
    | GU_TRANSFORM_3D  // use matrices
    | GU_VERTICES(2);  // 2 vertices per line

    sceGuDrawArray(GU_LINES, flags, count, indices, vertices);
}


void PSPRenderer::clear(Color color) {
    sceGuClearColor(color);
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void PSPRenderer::swapBuffers(void) {
    //set buffer todo
    sceGuSwapBuffers();
}

void PSPRenderer::waitForVSync(void) {
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
}

/*
namespace GFX {

void init(void) {
}

void clear(int color) {
}

void flip(void) {
}




typedef enum
{
    SDL_ScaleModeNearest, < nearest pixel sampling 
    SDL_ScaleModeLinear,  < linear filtering 
    SDL_ScaleModeBest     < anisotropic filtering 
} SDL_ScaleMode;

void drawRect(RECT<int> *Disp, float zoom, uint8_t r, uint8_t g, uint8_t b) {

}

template<typename T> void drawTex(const Texture* tex, RECT<int> *Img, RECT<T> *Disp, float angle, int alpha, float zoom) {
    RECT<T> zoomDisp = {static_cast<T>((Disp->x * zoom) + (app->renderer->screenwidth * (1 - zoom) / 2)), static_cast<T>((Disp->y * zoom) + (app->renderer->screenheight * (1 - zoom) / 2)), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};

    if (Disp->x+Disp->w >= 0 && Disp->x <= app->renderer->screenwidth && Disp->y+Disp->h >= 0 && Disp->y <= app->renderer->screenheight)
    {
    }
}

template void drawTex<int>(const Texture* tex, RECT<int> *Img, RECT<int> *Disp, float angle, int alpha, float zoom);
template void drawTex<float>(const Texture* tex, RECT<int> *Img, RECT<float> *Disp, float angle, int alpha, float zoom);

template<typename T> void drawColTex(const Texture* tex, RECT<int> *Img, RECT<T> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b) {
    RECT<T> zoomDisp = {static_cast<T>((Disp->x * zoom) + (app->renderer->screenwidth * (1 - zoom) / 2)), static_cast<T>((Disp->y * zoom) + (app->renderer->screenheight * (1 - zoom) / 2)), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};

    if (Disp->x+Disp->w >= 0 && Disp->x <= app->renderer->screenwidth && Disp->y+Disp->h >= 0 && Disp->y <= app->renderer->screenheight)
    {
    }
}

template void drawColTex<int>(const Texture* tex, RECT<int> *Img, RECT<int> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b);
template void drawColTex<float>(const Texture* tex, RECT<int> *Img, RECT<float> *Disp, float angle, int alpha, float zoom, uint8_t r, uint8_t g, uint8_t b);

};
*/