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
    selection = 0;
    data = app->assetmanager.get<JsonAsset>(getPath("assets/menu/story/storymenu.json").c_str());
}

void StoryModeScreen::update(void) 
{
    freaky->process();
    if (app->event.isPressed(Input::MENU_UP))
    {
        selection -= 1;
        if (selection < 0)
            selection = static_cast<int>(data->value["weeks"].size())-1;
    }
    else if (app->event.isPressed(Input::MENU_DOWN))    
    {
        selection += 1;
        if (selection > static_cast<int>(data->value["weeks"].size())-1)
            selection = 0;
    }
    if (app->event.isPressed(Input::MENU_ENTER))
    {
        setScreen(new PlayStateScreen(data->value["weeks"][selection]["songs"][0].asString(), false));
    }

    if (app->event.isPressed(Input::MENU_ESCAPE))
    {       
        setScreen(new MainMenuScreen(freaky->getPosition()));
    }
}

void StoryModeScreen::draw(void) 
{
    Gfx::RectWH<int> black_background = {0, 0, app->renderer->width, app->renderer->height};
    //GFX::drawrect(&black_background, 1, 0, 0, 0);
    Gfx::RectWH<int> middle_strap = {0, 21, app->renderer->width, 152};
    //GFX::drawrect(&middle_strap, 1, 247, 208, 81);

    app->normalFont->Print(Left,  5,   6, "WEEK SCORE: ");
    app->normalFont->Print(Left, 51, 191, "TRACKS");
    for (int i = 0; i < static_cast<int>(data->value["weeks"][selection]["songs"].size()); i++)
        app->normalFont->Print(Center, 51, 213+(10*i), data->value["weeks"][selection]["songs"][i].asString().c_str());
}

StoryModeScreen::~StoryModeScreen(void) 
{
    delete freaky;
    app->assetmanager.release(data->assetpath.c_str());
}