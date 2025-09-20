#include "trans.hpp"
#include "app.hpp"
#include "engine/renderer.hpp"

//todo horrible and needs a rewrite
namespace GAME {

void Transition::update(float time) {
    y.setTweenTime(time);
    dst.y = static_cast<int>(y.getValue());
    //reset 
    if (dst.y >= GFX::SCREEN_HEIGHT+TRANS_PIXELS){
//        printf("resseting transition\n");
        init(); 
    }
    if (dst.y >= 0 && !loadedassets) {
        if (loadFunction != nullptr)
            loadFunction(); //doesnt do async loading
        loadedassets = true;
    }
}

void Transition::draw(void) {
    if ((dst.y + dst.h) + TRANS_PIXELS <= 0) return; //offscreen

    for (int i = 0; i < TRANS_PIXELS; i++) {
        //top gradient
        uint8_t topAlpha = static_cast<uint8_t>((i / (float)TRANS_PIXELS) * 255.0f);
        GFX::RECT<int32_t> topGrad = { 0, dst.y - TRANS_PIXELS + i, dst.w, 1 };
        uint32_t topColor = (0x00 << 24) | (0x00 << 16) | (0x00 << 8) | topAlpha; // RGBA
        g_app.renderer->drawRect(topGrad, 0, topColor);

        //bottom gradient
        uint8_t bottomAlpha = static_cast<uint8_t>(((TRANS_PIXELS - i) / (float)TRANS_PIXELS) * 255.0f);
        GFX::RECT<int32_t> bottomGrad = { 0, dst.y + dst.h + i, dst.w, 1 };
        uint32_t bottomColor = (0x00 << 24) | (0x00 << 16) | (0x00 << 8) | bottomAlpha; // RGBA
        g_app.renderer->drawRect(bottomGrad, 0, bottomColor);
    }

    //main rect
    g_app.renderer->drawRect(dst, 0, 0x000000FF);

//    printf("drawing trans\n");
}

}