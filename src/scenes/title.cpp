#include "title.hpp"
#include "../engine/pc/renderersdl.hpp"
#include "../engine/hash.hpp"

TitleSCN::TitleSCN(void) {
    freaky = new Audio::StreamedFile(g_app.audiomixer, FS::getFilePath("assets/music/freaky.ogg").c_str());
    freaky->play(true);
    object.init(FS::getFilePath("assets/menu/gftitle/gfDanceTitle.json"));
    object.setAnim("gfDance"_h);
    object.playAnim(object.getCurIndicieCount()/object.getCurFps());
    object.setScale(0.975);
}

void TitleSCN::update(void) {
    freaky->process();
    object.update(g_app.timer.elapsedMS()/1000);
    if (!object.playing())
        object.playAnim(object.getCurIndicieCount()/object.getCurFps());
}

void TitleSCN::draw(void) {
    object.draw({0, 13});
}

TitleSCN::~TitleSCN(void) {
    delete freaky;
}  