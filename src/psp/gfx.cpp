#include "../main.h"
#include "gfx.h"
#include "../screen.h"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <vram.h>
#include "memory.h"

namespace GFX {

#define DLIST_SIZE              (524288)
#define LINE_SIZE               (512)
#define PIXEL_SIZE              (4)
#define FRAMEBUFFER_SIZE        (LINE_SIZE*SCREEN_HEIGHT*PIXEL_SIZE)
#define MALLOC_STEP             (128)
#define TSTACK_MAX              (64)
#define SLICE_WIDTH             (64.f)
#define M_180_PI                (57.29578f)
#define M_PI_180                (0.017453292f)

static int *dlist;
static Texture disp_buffer;
static Texture draw_buffer;

void init(void) {
    // Display list allocation
    dlist = (int*)Mem::pspf_malloc(DLIST_SIZE);

    // Setup GU
    sceGuInit();
    sceGuStart(GU_DIRECT, dlist);

    sceGuDrawBuffer(GU_PSM_8888, draw_buffer.texdata, LINE_SIZE);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, disp_buffer.texdata, LINE_SIZE);
    sceGuDepthBuffer((void*)(FRAMEBUFFER_SIZE*2), LINE_SIZE);
    sceGuOffset(2048-SCREEN_WIDTH/2, 2048-SCREEN_HEIGHT/2);
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    draw_buffer.texdata = (unsigned int*)vabsptr(draw_buffer.texdata);
    disp_buffer.texdata = (unsigned int*)vabsptr(disp_buffer.texdata);

    sceGuDepthRange(65535, 0);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuClearDepth(65535);
    sceGuAlphaFunc(GU_GREATER, 0, 255);
    sceGuDepthFunc(GU_LEQUAL);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_LINEAR, GU_LINEAR);
    sceGuShadeModel(GU_SMOOTH);

    sceGuDisable(GU_CULL_FACE);
    sceGuDisable(GU_CLIP_PLANES);
    sceGuDisable(GU_DITHER);
    sceGuEnable(GU_ALPHA_TEST);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuEnable(GU_BLEND);

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
    //start rendering
    sceKernelDcacheWritebackRange(dlist, DLIST_SIZE);
    sceGuStart(GU_DIRECT, dlist);


}

void clear(int color) {
    sceGuClearColor(color);
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);
}

void flip(void) {

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();

}

Texture *loadTex(const char *path) {
    return reinterpret_cast<Texture *>(1);
}

void freeTex(Texture **tex) {
    
}

template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, bool linear, float angle, int alpha) {
    ASSERTFUNC(tex, "texture is NULL");

    if (Disp->x+Disp->w >= 0 && Disp->x <= SCREEN_WIDTH && Disp->y+Disp->h >= 0 && Disp->y <= SCREEN_HEIGHT)
    {
    }
}

template void drawTex<int>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, bool linear, float angle, int alpha);
template void drawTex<float>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, bool linear, float angle, int alpha);

template<typename T> void drawTexZoom(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, bool linear, float angle, int alpha, float zoom) {
    ASSERTFUNC(tex, "texture is NULL");

    float x = (Disp->x - SCREEN_WIDTH/2) * zoom + SCREEN_WIDTH/2;
    float y = (Disp->y - SCREEN_HEIGHT/2) * zoom + SCREEN_HEIGHT/2;
    float w = Disp->w * zoom;
    float h = Disp->h * zoom;
    if (x+w >= 0 && x <= SCREEN_WIDTH && y+h >= 0 && y <= SCREEN_HEIGHT)
    {
    }
}

template void drawTexZoom<int>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, bool linear, float angle, int alpha, float zoom);
template void drawTexZoom<float>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, bool linear, float angle, int alpha, float zoom);

// void GFX::drawTex(GFX::Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, bool linear, float angle, int alpha)
// {   
//     ASSERTFUNC(tex, "texture is NULL");

//     if (Disp->x+Disp->w >= 0 && Disp->x <= GFX::SCREEN_WIDTH && Disp->y+Disp->h >= 0 && Disp->y <= GFX::SCREEN_HEIGHT)
//     {
//         g2dBeginRects(tex);
//         g2dSetCropXY(Img->x, Img->y);
//         g2dSetCropWH(Img->w, Img->h);
//         g2dSetCoordXY(Disp->x, Disp->y);
//         g2dSetScaleWH(Disp->w, Disp->h);
//         g2dSetTexLinear(linear);
//         g2dSetRotationRad(angle);
//         if (alpha >= 0)
//             g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, alpha));
//         else
//             g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, 0));
//         g2dAdd();
//         g2dEnd();
//     }
// }

// void GFX::drawTex(GFX::Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, bool linear, float angle, int alpha)
// {
//     ASSERTFUNC(tex, "texture is NULL");

//     if (Disp->x+Disp->w >= 0 && Disp->x <= GFX::SCREEN_WIDTH && Disp->y+Disp->h >= 0 && Disp->y <= GFX::SCREEN_HEIGHT)
//     {
//         g2dBeginRects(tex);
//         g2dSetCropXY(Img->x, Img->y);
//         g2dSetCropWH(Img->w, Img->h);
//         g2dSetCoordXY(Disp->x, Disp->y);
//         g2dSetScaleWH(Disp->w, Disp->h);
//         g2dSetTexLinear(linear);
//         g2dSetRotationRad(angle);
//         if (alpha >= 0)
//             g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, alpha));
//         else
//             g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, 0));
//         g2dAdd();
//         g2dEnd();
//     }
// }

// void DrawZoomG2DTex(GFX::Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, bool linear, float angle, int alpha, float zoom)
// {
//     ASSERTFUNC(tex, "texture is NULL");

//     float x = (Disp->x - GFX::SCREEN_WIDTH/2) * zoom + GFX::SCREEN_WIDTH/2;
//     float y = (Disp->y - GFX::SCREEN_HEIGHT/2) * zoom + GFX::SCREEN_HEIGHT/2;
//     float w = Disp->w * zoom;
//     float h = Disp->h * zoom;
//     if (x+w >= 0 && x <= GFX::SCREEN_WIDTH && y+h >= 0 && y <= GFX::SCREEN_HEIGHT)
//     {
//         g2dBeginRects(tex);
//         g2dSetCropXY(Img->x, Img->y);
//         g2dSetCropWH(Img->w, Img->h);
//         g2dSetCoordXY(x, y);
//         g2dSetScaleWH(w, h);
//         g2dSetTexLinear(linear);
//         g2dSetRotationRad(angle);
//         if (alpha >= 0)
//             g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, alpha));
//         else
//             g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, 0));
//         g2dAdd();
//         g2dEnd();
//     }
// }
};