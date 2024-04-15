#include "donate.h"
#include "../app.h"

#include "mainmenu.h"

DonateScreen::DonateScreen(void) {
    background = new GFX::Texture();
    background->load(getPath("assets/menu/back.png").c_str());
}

void DonateScreen::update(void) 
{
    if (app->event.isPressed(Input::MENU_ESCAPE))
    {
        setScreen(new MainMenuScreen());
    }
}

void DonateScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {GFX::SCREEN_WIDTH/2 - 700/2, GFX::SCREEN_HEIGHT/2 - 397/2, 700, 397};
    GFX::drawTex<float>(background, &background_img, &background_disp, 0, 255);
}

DonateScreen::~DonateScreen(void) 
{
    delete background;
}