#include "object.hpp"
#include "engine/assets.hpp"
#include "engine/renderer.hpp"


namespace OBJECT {


void Object2D::init(ASSETS::AssetManager &mgr, std::string path) {
    setScale(1);
    CAMERA::Camera defaultcam(0,0,0);
    setCamera(defaultcam);
}

void Object2D::update(float t) {
}

void Object2D::draw(GFX::Renderer *renderer, GFX::XY<int32_t> &pos) {
    auto frame = animator.getCurFrame();
    auto &src = frame->src;

    int32_t scw2 = GFX::SCREEN_WIDTH / 2;
    int32_t sch2 = GFX::SCREEN_HEIGHT / 2;

    //scale relative to screen center
    int32_t dstx = scw2 + (pos.x - scw2) * scale - frame->ox * scale;
    int32_t dsty = sch2 + (pos.y - sch2) * scale - frame->oy * scale;

    GFX::RECT<int32_t> dst = {dstx-cam.x, dsty-cam.y, static_cast<int32_t>(src.w * scale), static_cast<int32_t>(src.h * scale)};

    renderer->drawTexRect(animator.getCurTex()->image, src, dst, cam.z, 0xFFFFFFFF);
}

} //namespace OBJECT