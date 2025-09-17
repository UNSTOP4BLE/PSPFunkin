#include "rendererpsp.hpp"
#include <cassert>
#include "glib2d.hpp"
namespace GFX {

void PSPRenderer::init() {
    g2dInit();
}

bool PSPRenderer::running() {
    return true;
}

void PSPRenderer::beginFrame() {
    g2dClear(0xFFFFFF);
}

void PSPRenderer::endFrame() {
    g2dFlip(G2D_VSYNC);
}

//static void setColor(uint32_t col) {
//
//}

void PSPRenderer::drawRect(RECT<int32_t> rect, int z, uint32_t col) {

}

void PSPRenderer::drawTexRect(const Texture &tex, XY<int32_t> pos, int z, uint32_t col) {

    g2dBeginRects(tex.tex);
    g2dSetCropXY(0, 0);
    g2dSetCropWH(255, 255);
    g2dSetCoordXY(pos.x,pos.y);
//    g2dSetScaleWH(Disp->w, Disp->h);
    g2dSetTexLinear(false);
//    g2dSetRotationRad(angle);
//    if (alpha >= 0)
  //      g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, alpha));
    //else
      //  g2dSetColor(G2D_MODULATE(0xFFFFFFFF, 255, 0));
    g2dAdd();
    g2dEnd();
}

}