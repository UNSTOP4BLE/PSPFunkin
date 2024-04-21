#include "storymode.h"
#include "../app.h"
#include <dirent.h>
#include <stdio.h>

#include "mainmenu.h"

StoryModeScreen::StoryModeScreen(void) {
    background = new GFX::Texture();
    background->load(getPath("assets/menu/back.png").c_str());
    selection = 0;
}

void StoryModeScreen::update(void) 
{
    /*
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i]->tick();
        int frame = i;
        if (selection == i)
            frame += static_cast<int>(COUNT_OF(menu_selections));

        if (!menu_selections[i]->cantick)
            menu_selections[i]->setAnim(frame, ModeNone);
    }*/
   
    if (app->event.isPressed(Input::MENU_UP))
    {
        selection -= 1;
        if (selection < 0)
            selection = 0;
    }
    else if (app->event.isPressed(Input::MENU_DOWN))    
    {
        selection += 1;
//        if (selection > static_cast<int>(songs.size()-1))
  //          selection = static_cast<int>(songs.size()-1);
    }
    if (app->event.isPressed(Input::MENU_ENTER))
    {}

    if (app->event.isPressed(Input::MENU_ESCAPE))
    {
        setScreen(new MainMenuScreen());
    }
}

void StoryModeScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {GFX::SCREEN_WIDTH/2 - 700/2, GFX::SCREEN_HEIGHT/2 - 397/2, 700, 397};
    GFX::drawTex<float>(background, &background_img, &background_disp, 0, 255, 1);
}

StoryModeScreen::~StoryModeScreen(void) 
{
    delete background;
}