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
    GFX::RECT<int> black_background = {0, 0, app->screenwidth, app->screenheight};
    GFX::drawRect(&black_background, 1, 0, 0, 0);
    GFX::RECT<int> middle_strap = {0, 21, app->screenwidth, 152};
    GFX::drawRect(&middle_strap, 1, 247, 208, 81);

    app->normalFont->Print(Left,  5,   6, "WEEK SCORE: ");
    app->normalFont->Print(Left, 51, 191, "TRACKS");
    app->normalFont->Print(Center, 51, 213, "TEST");
    app->normalFont->Print(Center, 51, 213+10, "TESTTESTTEST");
    app->normalFont->Print(Left, 50,  50, "only week 1 currently available \n(press x to start)");
}

StoryModeScreen::~StoryModeScreen(void) 
{
    delete freaky;
}