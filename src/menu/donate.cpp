#include "donate.h"
#include "../app.h"
#include "../psp/font.h"

#include "mainmenu.h"

DonateScreen::DonateScreen(int songpos) {
    freaky = new Audio::StreamedFile(*app->audioMixer, getPath("assets/music/freaky/freaky.ogg").c_str());
    freaky->play(true);
    freaky->setPosition(songpos);
    background = app->assetmanager.get<ImageAsset>(getPath("assets/menu/back.png").c_str());
    back = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/cancelMenu.wav").c_str());
}

void DonateScreen::update(void) 
{
    freaky->process();

    if (app->event.isPressed(Input::MENU_ESCAPE))
    {
        app->audioMixer->playBuffer(back->soundbuffer);
        setScreen(new MainMenuScreen(freaky->getPosition()));
    }
}

void DonateScreen::draw(void) 
{
    RECT<int> background_img = {0, 0, 512, 331};
    RECT<float> background_disp = {app->renderer->screenwidth/2 - 700/2, app->renderer->screenheight/2 - 397/2, 700, 397};
    //GFX::drawTex<float>(&background->image, &background_img, &background_disp, 0, 255, 1);

    app->normalFont->Print(Center, app->renderer->screenwidth/2, app->renderer->screenheight/2, "DONATE TO ME IF YOU LIKE MY WORKS!!");
    app->normalFont->Print(Center, app->renderer->screenwidth/2, app->renderer->screenheight/2+11, "paypal.me/johnathanthepork16th");
}

DonateScreen::~DonateScreen(void) 
{
    app->assetmanager.release(background->assetpath.c_str());
    delete freaky;
    app->assetmanager.release(back->assetpath.c_str());
}