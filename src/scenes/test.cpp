#include "test.hpp"
#include "../engine/file.hpp"

TestSCN::TestSCN(void) {
    g_app.renderer->setClearCol(0x00FF00FF);

    obj.init(g_app.assets, FS::getFilePath("assets/menu/gftitle/gf.json"));
}

void TestSCN::update(void) {
    obj.update(g_app.timer.elapsedS());
    if (!obj.isPlaying()) {
        obj.setDuration(ANIMATION::calcDuration_FPS(obj.getIndicieCount(), obj.getFPS()));
        obj.playAnim();
    }
}

void TestSCN::draw(void) {
    GFX::RECT<int32_t> src = obj.curkeyframe->src;
    GFX::RECT<int32_t> dst = {128-obj.curkeyframe->ox, 128-obj.curkeyframe->oy, src.w, src.h};

    g_app.renderer->drawTexRect(obj.curtex->tex->image, src, dst, 0, 0xFFFFFFFF);
}

TestSCN::~TestSCN(void) {
}  