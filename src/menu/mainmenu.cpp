#include "mainmenu.h"
#include "../main.h"
#include "../screen.h"
#include "../psp/font.h"
#include "../app.h"

#include "../playstate.h"

MainMenuScreen::MainMenuScreen(void) {
    setScreenCol(0xFF00FF00);
    background = GFX::loadTex("assets/menu/back.png");
    backgroundy.setValue(GFX::SCREEN_HEIGHT/2 - 331/2);
    selection = 0;
    songs[0] = "bopeebo";
    songs[1] = "fresh (currently not implemented)";
    songs[2] = "test";
    songs[3] = "no-more-woah";
    selectedsong = songs[0];
}

void MainMenuScreen::update(void) 
{
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
        if (selection > static_cast<int>(COUNT_OF(songs)-1))
            selection = static_cast<int>(COUNT_OF(songs)-1);
        backgroundy.setValue(400, 1.0);
    }
    selectedsong = songs[selection];

    if (app->event.isPressed(Input::MENU_ENTER))
        setScreen(new PlayStateScreen(selectedsong));
}

void MainMenuScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {GFX::SCREEN_WIDTH/2 - 583/2, backgroundy.getValue(), 583, 331};
    GFX::drawTex<float>(background, &background_img, &background_disp, 0, 255);
    PrintFont(Left, 0, 20, "press up and down to switch song, song: %s", selectedsong.c_str());
}

MainMenuScreen::~MainMenuScreen(void) 
{
    GFX::freeTex(background);
}