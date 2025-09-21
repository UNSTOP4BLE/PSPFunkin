#include "mainmenu.hpp"
#include "../engine/file.hpp"
#include "../engine/hash.hpp"
#include <algorithm>

#include "freeplay.hpp"

static void updateAnims(std::vector<Option> &menuoptions, int selection) {
    for (int i = 0; i < menuoptions.size(); i++) {
        auto &option = menuoptions[i];
        std::string animname = " basic";
        if (i == selection)
            animname = " white";
        uint32_t animhash = Hash::FromString((option.name + animname).c_str());
        //set to selected animation
        //option.obj.playAnim(animhash);
    }
}

MainMenuSCN::MainMenuSCN(void) {
    g_app.renderer->setClearCol(0x00FF00FF);
    
    menubg =  g_app.assets.get<ASSETS::ImageAsset>(FS::getFilePath("assets/menu/menu.png").c_str());

    //must match up the animation name in xml
    struct {
        std::string name;
        GFX::XY<int32_t> pos;
    } options[] = {
        { "story mode", {246/2, -25} },
        { "freeplay",   {194/2, -25} },
        { "options",    {196/2, -25} },
        { "donate",     {176/2, -20} }
    };
    std::string atlaspath = FS::getFilePath("assets/menu/selections.json");
    
    //initialize
    for (auto &option : options) {
        menuoptions.emplace_back();
        auto &opt = menuoptions.back();
        opt.name = option.name;
        opt.offset = option.pos;
       // opt.obj.init(g_app.assets, atlaspath);
       // opt.obj.setLoop(true);
    }
    selection = 0;
    updateAnims(menuoptions, selection);
    menubgy.setValue(0);
}

constexpr float MENUBG_SPEED = 0.4;
constexpr float MENUBG_AMOUNT = 8;

void loadFreePlay(void) {
    SCENE::set(new FreePlaySCN());
}

void MainMenuSCN::update(void) {
    menubgy.setTweenTime(g_app.timer.elapsedMS()/1000);
    if (g_app.input.isPressed(INPUT::MENU_UP)) {
        selection --;
        selection = std::clamp(selection, 0, static_cast<int>(menuoptions.size()-1));
        updateAnims(menuoptions, selection);
        menubgy.setValue(selection*MENUBG_AMOUNT, MENUBG_SPEED);
        g_app.assets.printLoadedAssets();
    } else if (g_app.input.isPressed(INPUT::MENU_DOWN)) {
        selection ++;
        selection = std::clamp(selection, 0, static_cast<int>(menuoptions.size()-1));
        updateAnims(menuoptions, selection);
        menubgy.setValue(selection*MENUBG_AMOUNT, MENUBG_SPEED);
    }
    for (auto &option : menuoptions) {
        //option.obj.update(g_app.timer);
    }

    //handle selection
    if (g_app.input.isPressed(INPUT::MENU_ENTER)) {
        switch (Hash::FromString(menuoptions[selection].name.c_str())) {
            case "story mode"_h: 
                printf("hi\n");
                break;

            case "freeplay"_h: 
                g_app.trans.init(loadFreePlay);
                g_app.trans.start();
                break;

            case "options"_h: 
                printf("hi\n");
                break;
            default:
                break;
        }
    }
}

void MainMenuSCN::draw(void) {
    //menubg
    GFX::RECT<int32_t> src = {0, 0, 480, 272};
    float scale = 1.1;

    int width = GFX::SCREEN_WIDTH;
    int height = GFX::SCREEN_HEIGHT;
    int cx = width / 2;
    int cy = height / 2;

    //scale around center
    GFX::RECT<int32_t> dst = {
        static_cast<int>(cx - (width * scale) / 2),
        -static_cast<int>(menubgy.getValue()),
        static_cast<int>(width * scale),
        static_cast<int>(height * scale)
    };
    g_app.renderer->drawTexRect(menubg->image, src, dst, 0, 0xFCE871FF);

    for (int i = menuoptions.size()-1; i >= 0; i--) { //draw different layering
        auto &option = menuoptions[i];
        auto o = option.offset;
        if (selection == i) { //offset for selected sprite
            o.x += o.x/4;
            o.y -= o.y/4;
        }
   //     option.obj.draw(g_app.renderer, {(GFX::SCREEN_WIDTH/2)-o.x, (60*i)-o.y});
    }
}

MainMenuSCN::~MainMenuSCN(void) {
}  