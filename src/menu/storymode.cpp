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
    loadJson(getPath("assets/menu/story/storymenu.json").c_str(), &data);
}

void StoryModeScreen::update(void) 
{
    freaky->process();
    if (app->event.isPressed(Input::MENU_UP))
    {
        selection -= 1;
        if (selection < 0)
            selection = static_cast<int>(data["weeks"].size())-1;
    }
    else if (app->event.isPressed(Input::MENU_DOWN))    
    {
        selection += 1;
        if (selection > static_cast<int>(data["weeks"].size())-1)
            selection = 0;
    }
    if (app->event.isPressed(Input::MENU_ENTER))
    {
        setScreen(new PlayStateScreen(data["weeks"][selection]["songs"][0].asString(), false));
    }

    if (app->event.isPressed(Input::MENU_ESCAPE))
    {
        setScreen(new MainMenuScreen(freaky->getPosition()));
    }
}

void StoryModeScreen::draw(void) 
{
    GFX::RECT<int> black_background = {0, 0, app->screenwidth, app->screenheight};
    GFX::drawRect(&black_background, 1, 0, 0, 0);
    GFX::RECT<int> middle_strap = {0, 21, app->screenwidth, 152};
    GFX::drawRect(&middle_strap, 1, 247, 208, 81);

    app->normalFont->Print(Left,  5,   6, "WEEK SCORE: ");
    app->normalFont->Print(Left, 51, 191, "TRACKS");
    for (int i = 0; i < static_cast<int>(data["weeks"][selection]["songs"].size()); i++)
        app->normalFont->Print(Center, 51, 213+(10*i), data["weeks"][selection]["songs"][i].asString().c_str());
    
    app->normalFont->Print(Left, 50,  50, "only week 1 currently available \n(press x to start)");
}

StoryModeScreen::~StoryModeScreen(void) 
{
    delete freaky;
}