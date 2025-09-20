#include "mainmenu.hpp"
#include "../engine/file.hpp"
#include "../engine/hash.hpp"
#include <algorithm>

static void updateAnims(std::vector<Option> &menuoptions, int selection) {
    for (int i = 0; i < menuoptions.size(); i++) {
        auto &option = menuoptions[i];
        std::string animname = " basic";
        if (i == selection)
            animname = " white";
        uint32_t animhash = Hash::FromString((option.name + animname).c_str());
        option.obj.playAnim(animhash);
    }
}

MainMenuSCN::MainMenuSCN(void) {
    g_app.renderer->setClearCol(0x00FF00FF);
    
    //must match up the animation name in xml

    struct {
        std::string name;
        GFX::XY<int32_t> pos;
    } options[] = {
        { "story mode", {246/2, 0} },
        { "freeplay",   {194/2, 0} },
        { "options",    {196/2, 0} },
        { "donate",     {176/2, 5} }
    };
    std::string atlaspath = FS::getFilePath("assets/menu/selections.json");
    //initialize
    for (auto &option : options) {
        menuoptions.emplace_back();
        auto &opt = menuoptions.back();
        opt.name = option.name;
        opt.offset = option.pos;
        opt.obj.init(atlaspath);
        opt.obj.setLoop(true);
    }
    selection = 0;
    updateAnims(menuoptions, selection);
}

void MainMenuSCN::update(void) {
    if (g_app.input.isPressed(INPUT::MENU_UP)) {
        selection --;
        selection = std::clamp(selection, 0, static_cast<int>(menuoptions.size()-1));
        updateAnims(menuoptions, selection);
    } else if (g_app.input.isPressed(INPUT::MENU_DOWN)) {
        selection ++;
        selection = std::clamp(selection, 0, static_cast<int>(menuoptions.size()-1));
        updateAnims(menuoptions, selection);
    }
    for (auto &option : menuoptions) {
        option.obj.update(g_app.timer);
    }
}

void MainMenuSCN::draw(void) {
    for (int i = menuoptions.size()-1; i >= 0; i--) { //draw different layering
        auto &option = menuoptions[i];
        auto o = option.offset;
        if (selection == i) {
            o.x += o.x/4;
            o.y += o.y/4;
        }
        option.obj.draw(g_app.renderer, {(GFX::SCREEN_WIDTH/2)-o.x, (60*i)-o.y});
    }
}

MainMenuSCN::~MainMenuSCN(void) {
}  