#include "title.hpp"

#include "../engine/hash.hpp"
#include "../engine/file.hpp"

#include "mainmenu.hpp"

TitleSCN::TitleSCN(void) {
    g_app.renderer->setClearCol(0x000000FF);
  //  g_app.audiomanager.loadFile(g_app.audiomixer, FS::getFilePath("assets/music/freaky.ogg"));
//    g_app.audiomanager.play(true);

    auto jdata = FS::readJsonFile(FS::getFilePath("assets/menu/menu.json"));
    float crochet = (60 / jdata["menuSongBPM"].asFloat()) * 1000;
    stepcrochet = crochet / 4;

    auto msgs = jdata["messages"];
    int curmsg = rand() % (msgs.size()); //get a random message
    funnymessage[0] = msgs[curmsg][0].asString();
    funnymessage[1] = msgs[curmsg][1].asString();

    printf("%s %s \n", funnymessage[0].c_str(), funnymessage[1].c_str()); //todo display
    //dancing gf
    gf.init(g_app.assets, FS::getFilePath("assets/menu/gftitle/gf.json"));
    gf.setLoop(true);
    gf.setScale(0.975);
    //fnf logo, todo make this step based instead
    logo.init(g_app.assets, FS::getFilePath("assets/menu/logo.json"));
    logo.setLoop(true);
    logo.setScale(0.975);
    //press enter
    pressenter.init(g_app.assets, FS::getFilePath("assets/menu/titleenter.json"));
    pressenter.playAnim("Press Enter to Begin"_h);
    pressenter.setLoop(true);
    pressenter.setScale(0.975);
}

static void loadMainMenu(void) {
    SCENE::set(new MainMenuSCN());
}

void TitleSCN::update(void) {
//    curstep =  g_app.audiomanager.getMS() / stepcrochet;
    gf.update(g_app.timer);
    logo.update(g_app.timer);
    pressenter.update(g_app.timer);
    if (g_app.input.isPressed(INPUT::MENU_ENTER)) {
        g_app.trans.init(loadMainMenu);
        g_app.trans.start();
    }
}

void TitleSCN::draw(void) {
    gf.draw(g_app.renderer, {200, 13});
    logo.draw(g_app.renderer, {-3, 3});
    pressenter.draw(g_app.renderer, {GFX::SCREEN_WIDTH/2-200, GFX::SCREEN_HEIGHT * 5 / 6});
}

TitleSCN::~TitleSCN(void) {
    gf.free();
    logo.free();
    pressenter.free();
}  