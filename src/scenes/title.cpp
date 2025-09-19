#include "title.hpp"
#include "../engine/pc/renderersdl.hpp"
#include "../engine/hash.hpp"

TitleSCN::TitleSCN(void) {
    g_app.renderer->setClearCol(0x000000FF);
    freaky = new Audio::StreamedFile(g_app.audiomixer, FS::getFilePath("assets/music/freaky.ogg").c_str());
    freaky->play(true);
    //dancing gf
    gf.init(FS::getFilePath("assets/menu/gftitle/gf.json"));
    gf.setAnim("gfDance"_h);
    gf.playAnim(gf.getCurIndicieCount()/gf.getCurFps());
    gf.setScale(0.975);
    //fnf logo
    logo.init(FS::getFilePath("assets/menu/logo.json"));
    logo.setAnim("logo bumpin"_h);
    logo.playAnim(logo.getCurIndicieCount()/logo.getCurFps());
    logo.setScale(0.975);
    //press enter
    pressenter.init(FS::getFilePath("assets/menu/titleenter.json"));
    pressenter.setAnim("Press Enter to Begin"_h);
    pressenter.playAnim(pressenter.getCurIndicieCount()/pressenter.getCurFps());
    pressenter.setScale(0.975);
}

void TitleSCN::update(void) {
    freaky->process();
    gf.update(g_app.timer.elapsedMS()/1000);
    if (!gf.playing()) //seamless looping
        gf.playAnim(gf.getCurIndicieCount()/gf.getCurFps());
    logo.update(g_app.timer.elapsedMS()/1000);
    if (!logo.playing()) 
        logo.playAnim(logo.getCurIndicieCount()/logo.getCurFps());
    pressenter.update(g_app.timer.elapsedMS()/1000);
    if (!pressenter.playing()) 
        pressenter.playAnim(pressenter.getCurIndicieCount()/pressenter.getCurFps());
}

void TitleSCN::draw(void) {
    gf.draw({200, 13});
    logo.draw({-3, 3});
    pressenter.draw({GFX::SCREEN_WIDTH/2-200, GFX::SCREEN_HEIGHT * 5 / 6});
}

TitleSCN::~TitleSCN(void) {
    delete freaky;
    gf.free();
    logo.free();
}  