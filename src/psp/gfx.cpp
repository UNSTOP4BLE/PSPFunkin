#include "../main.h"
#include "gfx.h"
#include "../screen.h"

#ifdef PSP
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <png.h>
#include <vram.h>
#else
#include <SDL2/SDL_image.h>
#define PCSCALE 3 // works by multiplying psp screen res by this number
#endif

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
#ifdef PSP
static Texture disp_buffer;
static Texture draw_buffer;
#endif

void init(void) {
#ifdef PSP
    // Display list allocation
    dlist = (int*)Mem::pspf_malloc(DLIST_SIZE);

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

    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
#else    
    app->window = SDL_CreateWindow("PSPFunkin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * PCSCALE, SCREEN_HEIGHT * PCSCALE, SDL_WINDOW_SHOWN);
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    app->screenSurface = SDL_GetWindowSurface(app->window);

#endif
}

void clear(int color) {
#ifdef PSP
    sceKernelDcacheWritebackRange(dlist, DLIST_SIZE);
    sceGuStart(GU_DIRECT, dlist);
    sceGuClearColor(color);
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);
#else
    SDL_SetRenderDrawColor(app->renderer, ((color >> 16) & 0xFF) / 255.0, ((color >> 8) & 0xFF) / 255.0, ((color) & 0xFF) / 255.0, 255);
    SDL_RenderClear(app->renderer);
#endif
}

void flip(void) {
#ifdef PSP
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();

    disp_buffer.texdata = draw_buffer.texdata;
    draw_buffer.texdata = (unsigned int*)vabsptr(sceGuSwapBuffers());
#else
    SDL_RenderPresent(app->renderer);
#endif
}

Texture *loadTex(const char *path) {
#ifdef PSP
    /*
    Texture *texpsp;
    SDL_Surface *tex = SDL_LoadPNG(path);
    assert(tex);
    texpsp.texdata = tex;
    return 
    FILE *fp = fopen(path, "rb");
    ASSERTFUNC(fp, "failed to load png file");
    fclose(fp);*/
    return reinterpret_cast<Texture *>(1);
#else
    Texture *psptex = NULL;
    psptex = IMG_LoadTexture(app->renderer, path);
    ASSERTFUNC(psptex, "texture is NULL");
    return psptex;
#endif
}

void freeTex(Texture **tex) {
#ifdef PSP
#else
    //SDL_DestroyTexture(**tex->texdata);
#endif
}

template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, bool linear, float angle, int alpha) {
    ASSERTFUNC(tex, "texture is NULL");

    if (Disp->x+Disp->w >= 0 && Disp->x <= SCREEN_WIDTH && Disp->y+Disp->h >= 0 && Disp->y <= SCREEN_HEIGHT)
    {
        #ifdef PSP
        #else
        SDL_Rect _img = {static_cast<int>(Img->x), static_cast<int>(Img->y), static_cast<int>(Img->w), static_cast<int>(Img->h)};
        SDL_Rect _disp = {static_cast<int>(Disp->x) * PCSCALE, static_cast<int>(Disp->y) * PCSCALE, static_cast<int>(Disp->w) * PCSCALE, static_cast<int>(Disp->h) * PCSCALE};
        ASSERTFUNC(SDL_RenderCopy(app->renderer, tex, &_img, &_disp) >= 0, "failed to display sprite");
        #endif
    }
}

template void drawTex<int>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<int> *Disp, bool linear, float angle, int alpha);
template void drawTex<float>(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<float> *Disp, bool linear, float angle, int alpha);

template<typename T> void drawTexZoom(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, bool linear, float angle, int alpha, float zoom) {
    //no need to check if the texture is null cus drawTex() does it
    RECT<T> zoomDisp = {static_cast<T>((Disp->x - SCREEN_WIDTH/2) * zoom + SCREEN_WIDTH/2), static_cast<T>((Disp->y - SCREEN_HEIGHT/2) * zoom + SCREEN_HEIGHT/2), static_cast<T>(Disp->w * zoom), static_cast<T>(Disp->h * zoom)};
    drawTex(tex, Img, &zoomDisp, false, angle, alpha);
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