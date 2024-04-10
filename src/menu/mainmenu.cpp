#include "mainmenu.h"
#include "../main.h"
#include "../screen.h"
#include "../psp/font.h"
#include "../app.h"

#include <dirent.h>
#include <stdio.h>

#include "../playstate.h"

MainMenuScreen::MainMenuScreen(void) {
    setScreenCol(0xFF00FF00);
    background = new GFX::Texture();
    background->load("assets/menu/back.png");
    backgroundy.setValue(GFX::SCREEN_HEIGHT/2 - 331/2);
    selection = 0;

    const char* PATH = "assets/songs";

    DIR *dir = opendir(PATH);

    struct dirent *entry = readdir(dir);

    while (entry != NULL)
    {
        std::string name(entry->d_name);
        printf("song(%s)\n", name.c_str());
        songs.push_back(name);

        entry = readdir(dir);
    }

    closedir(dir);

    //initialize menu select sprites
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i] = new Anim_OBJECT("assets/menu/", "menuassets.json");
        menu_selections[i]->setAnim(0, ModeNone);
    }


//    selectedsong = songs[0];
}

void MainMenuScreen::update(void) 
{

    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i]->tick();
        int frame = i;
        if (i != 0)
            frame = 0;

        if (!menu_selections[i]->cantick)
            menu_selections[i]->setAnim(frame, ModeNone);
    }
    
/*
    if (app->event.isPressed(Input::MENU_DOWN))
    {
        selection -= 1;
        if (selection < 0)
            selection = 0;
        backgroundy.setValue(0, 1.0);
    }
    else if (app->event.isPressed(Input::MENU_UP))    
    {
        selection += 1;
        if (selection > static_cast<int>(songs.size()-1))
            selection = static_cast<int>(songs.size()-1);
        backgroundy.setValue(400, 1.0);
    }
    selectedsong = songs[selection];

    if (app->event.isPressed(Input::MENU_ENTER))
        setScreen(new PlayStateScreen(selectedsong)); */
}

void MainMenuScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {GFX::SCREEN_WIDTH/2 - 583/2, backgroundy.getValue(), 583, 331};
    GFX::drawTex<float>(background, &background_img, &background_disp, 0, 255);

    int y = 0;
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i]->draw(GFX::SCREEN_WIDTH/2 - menu_selections[i]->frames[menu_selections[i]->curframe].w/2, y + (GFX::SCREEN_HEIGHT/2 - menu_selections[i]->frames[menu_selections[i]->curframe].h/2), 0, 255, 1);
        y += 50;
    }

    PrintFont(Left, 0, 20, "press up and down to switch song, song: %s", selectedsong.c_str());
}

MainMenuScreen::~MainMenuScreen(void) 
{
    delete background;
}