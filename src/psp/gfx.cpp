//
// most of the psp graphics code is made by pyroesp https://github.com/pyroesp/PSP_Post_It_Editor
//
#include "../main.h"
#include "gfx.h"
#include "../screen.h"

//#ifdef PSP
//#include <pspkernel.h>
//#include <pspdisplay.h>
//#include <pspgu.h>
//#include <png.h>
//#include <vram.h>
//#else
#include <SDL2/SDL_image.h>
#define PCSCALE 1 // works by multiplying psp screen res by this number
//#endif

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
/*
#ifdef PSP
static int *dlist;
static Texture disp_buffer = {
    512, 512,
    SCREEN_WIDTH, SCREEN_HEIGHT, 
    0,
    0,
    0,
    0
};

static Texture draw_buffer = {
    512, 512,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    0,
    0,
    0,
    reinterpret_cast<uint32_t *>FRAMEBUFFER_SIZE
};
static uint32_t *vram = reinterpret_cast<uint32_t *>(0x40000000 | 0x04000000);
#endif
*/
void init(void) {
    /*
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
    
    draw_buffer.texdata = static_cast<uint32_t *>(vabsptr(draw_buffer.texdata));
    disp_buffer.texdata = static_cast<uint32_t *>(vabsptr(disp_buffer.texdata));

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
#else    */
    app->window = SDL_CreateWindow("PSPFunkin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * PCSCALE, SCREEN_HEIGHT * PCSCALE, SDL_WINDOW_SHOWN);
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    app->screenSurface = SDL_GetWindowSurface(app->window);

//#endif
}

void clear(int color) {
//#ifdef PSP
  //  sceKernelDcacheWritebackRange(dlist, DLIST_SIZE);
   // sceGuStart(GU_DIRECT, dlist);
   // sceGuClearColor(color);
    //sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);
//#else
    SDL_SetRenderDrawColor(app->renderer, ((color >> 16) & 0xFF) / 255.0, ((color >> 8) & 0xFF) / 255.0, ((color) & 0xFF) / 255.0, 255);
    SDL_RenderClear(app->renderer);
//#endif
}

void flip(void) {
/*
#ifdef PSP
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();

    disp_buffer.texdata = draw_buffer.texdata;
    draw_buffer.texdata = static_cast<uint32_t *>(vabsptr(sceGuSwapBuffers()));

    vram += FRAMEBUFFER_SIZE / sizeof(uint32_t);
#else*/
    SDL_RenderPresent(app->renderer);
//#endif
}

static int getNextPower2(int width){
    int b = width;
    int n;
    for (n = 0; b != 0; n++) 
        b >>= 1;
    b = 1 << n;
    if (b == 2 * width) 
        b >>= 1;
    return b;
}

Texture *loadTex(const char *path) {
    /*
#ifdef PSP
    png_structp png_ptr;
    png_infop info_ptr;
    
    FILE *fp = NULL;
    fp = fopen(path, "rb");
    if (!fp)
        return NULL;
    
    Texture* img = NULL;
    img = static_cast<Texture *>(Mem::pspf_malloc(sizeof(Texture)));
    
    if (!img)
        return NULL;
    
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        Mem::pspf_free(img);
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    // Determine the PNG image dimensions and format
    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    
    if (width > 512 || height > 512){
        Mem::pspf_free(img);
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    img->bit_depth = bit_depth;
    img->color_type = color_type;
    img->imgW = width;
    img->imgH = height;
    img->texW = getNextPower2(width);
    img->texH = height;
        
    // Allocate memory for the final image data
    uint32_t *data = static_cast<uint32_t *>(Mem::pspf_malloc(sizeof(uint32_t) * img->texW * height));
    
    if (!data){
        Mem::pspf_free(img);
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    
    img->texdata = data;
    img->rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate memory for the PNG image
    png_bytep *row_pointers = (png_bytep*)png_malloc(png_ptr, sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++)
        row_pointers[y] = (png_byte*)Mem::pspf_malloc(png_get_rowbytes(png_ptr, info_ptr));
    
    // Read the PNG image data
    png_read_image(png_ptr, row_pointers);

    // Copy the PNG image data to the final image data
    for (int y = 0; y < height; y++){
        png_bytep row = row_pointers[y];
        for (int x = 0; x < width; x++){
            png_bytep pixel = &(row[x * 3]);
            uint32_t agbr = (0xFF000000) | (static_cast<uint32_t>(pixel[2]) << 16) | (static_cast<uint32_t>(pixel[1] << 8)) | (static_cast<uint32_t>(pixel[0]));
            data[y * img->texW + x] = agbr;
        }
    }

    // Free memory allocated for the PNG image
    for (int y = 0; y < height; y++)
        Mem::pspf_free(row_pointers[y]);
    Mem::pspf_free(row_pointers);
    
    // Close the PNG file and clean up memory and resources
    fclose(fp);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    
    return img;
#else*/
    Texture *psptex = NULL;
    psptex = IMG_LoadTexture(app->renderer, path);
    ASSERTFUNC(psptex, "texture is NULL");
    return psptex;
//#endif
}

void freeTex(Texture *tex) {
//#ifdef PSP
//#else
    SDL_DestroyTexture(tex);
//#endif
}

template<typename T> void drawTex(Texture* tex, GFX::RECT<int> *Img, GFX::RECT<T> *Disp, bool linear, float angle, int alpha) {
    ASSERTFUNC(tex, "texture is NULL");

    if (Disp->x+Disp->w >= 0 && Disp->x <= SCREEN_WIDTH && Disp->y+Disp->h >= 0 && Disp->y <= SCREEN_HEIGHT)
    {
        /*
#ifdef PSP
        sceKernelDcacheWritebackInvalidateAll();
        sceGuStart(GU_DIRECT, dlist);
        sceGuCopyImage(GU_PSM_8888, Img->x, Img->y, Img->w, Img->h, tex->texW, tex->texdata, Disp->x, Disp->y, LINE_SIZE, vram);
        sceGuFinish();
        sceGuSync(0,0);
        */
 //   #else
        SDL_Rect _img = {static_cast<int>(Img->x), static_cast<int>(Img->y), static_cast<int>(Img->w), static_cast<int>(Img->h)};
        SDL_Rect _disp = {static_cast<int>(Disp->x) * PCSCALE, static_cast<int>(Disp->y) * PCSCALE, static_cast<int>(Disp->w) * PCSCALE, static_cast<int>(Disp->h) * PCSCALE};
        ASSERTFUNC(SDL_RenderCopy(app->renderer, tex, &_img, &_disp) >= 0, "failed to display sprite");
   //     #endif
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