#include "title.hpp"
#include "../engine/pc/renderersdl.hpp"
#include "../engine/hash.hpp"
#include "playstate.hpp"

void newscenetest(void) {
    SCENE::set(new PlayStateSCN);
}

TitleSCN::TitleSCN(void) {
//    g_app.renderer->setClearCol(0x000000FF);
    freaky = new Audio::StreamedFile(g_app.audiomixer, FS::getFilePath("assets/music/freaky.ogg").c_str());
    freaky->play(true);
    auto jdata = FS::readJsonFile(FS::getFilePath("assets/menu/menu.json"));
    float crochet = (60 / jdata["menuSongBPM"].asFloat()) * 1000;
    stepcrochet = crochet / 4;

    auto msgs = jdata["messages"];
    int curmsg = rand() % (msgs.size()); //get a random message
    funnymessage[0] = msgs[curmsg][0].asString();
    funnymessage[1] = msgs[curmsg][1].asString();

    printf("%s %s \n", funnymessage[0].c_str(), funnymessage[1].c_str()); //todo display
    //dancing gf
    gf.init(FS::getFilePath("assets/menu/gftitle/gf.json"));
    gf.setLoop(true);
    gf.setScale(0.975);
    //fnf logo, todo make this step based instead
    logo.init(FS::getFilePath("assets/menu/logo.json"));
    logo.setLoop(true);
    logo.setScale(0.975);
    //press enter
    pressenter.init(FS::getFilePath("assets/menu/titleenter.json"));
    pressenter.playAnim("Press Enter to Begin"_h);
    pressenter.setLoop(true);
    pressenter.setScale(0.975);

    g_app.trans.init(newscenetest);
    g_app.trans.start();
}

void TitleSCN::update(void) {
    curstep = freaky->getChannel().getTime()*1000 / stepcrochet;
    freaky->process();
    gf.update();
    logo.update();
    pressenter.update();
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
    pressenter.free();
}  