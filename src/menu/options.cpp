#include "options.h"
#include "../app.h"

#include "mainmenu.h"

OptionsScreen::OptionsScreen(int songpos) {
    freaky = new Audio::StreamedFile(*app->audioMixer, getPath("assets/music/freaky/freaky.ogg").c_str());
    freaky->play(true);
    freaky->setPosition(songpos);
    background = app->assetmanager.get<ImageAsset>(getPath("assets/menu/back.png").c_str());
    selection = 0;
}

void OptionsScreen::update(void) 
{
    freaky->process();
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
        setScreen(new MainMenuScreen(freaky->getPosition()));
    }
}

void OptionsScreen::draw(void) 
{
    Gfx::RectWH<int> background_img = {0, 0, 512, 331};
    Gfx::RectWH<int> background_disp = {app->renderer->width/2 - 700/2, app->renderer->height/2 - 397/2, 700, 397};
    //GFX::drawTex<float>(&background->image, &background_img, &background_disp, 0, 255, 1);
}

OptionsScreen::~OptionsScreen(void) 
{
    app->assetmanager.release(background->assetpath.c_str()); 
    delete freaky;
}