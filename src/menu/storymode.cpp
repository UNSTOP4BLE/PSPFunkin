#include "storymode.h"
#include "../app.h"
#include <dirent.h>
#include <stdio.h>

#include "mainmenu.h"
#include "../playstate.h"

StoryModeScreen::StoryModeScreen(int songpos) {
    freaky = new Audio::StreamedFile(*app->audioMixer, getPath("assets/music/freaky/freaky.ogg").c_str());
    freaky->play(true);
    freaky->setPosition(songpos);
    background = new GFX::Texture();
    background->load(getPath("assets/menu/back.png").c_str());
    selection = 0;
}

void StoryModeScreen::update(void) 
{
    freaky->process();
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
    {
        setScreen(new PlayStateScreen("bopeebo", false));
    }

    if (app->event.isPressed(Input::MENU_ESCAPE))
    {
        setScreen(new MainMenuScreen(freaky->getPosition()));
    }
}

void StoryModeScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {app->screenwidth/2 - 700/2, app->screenheight/2 - 397/2, 700, 397};
    GFX::drawTex<float>(background, &background_img, &background_disp, 0, 255, 1);

    app->normalFont->Print(Left, 50, 50, "only week 1 currently available \n(press x to start)");
}

StoryModeScreen::~StoryModeScreen(void) 
{
    delete background;
    delete freaky;
}