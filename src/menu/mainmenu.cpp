#include "mainmenu.h"
#include "../main.h"
#include "../app.h"

#include "title.h"
#include "storymode.h"
#include "freeplay.h"
#include "options.h"
#include "donate.h"

#define MENU_SPACING 50
#define BACKGROUND_SPEED 0.2

MainMenuScreen::MainMenuScreen(int songpos) {
    freaky = new Audio::StreamedFile(*app->audioMixer, getPath("assets/music/freaky/freaky.ogg").c_str());
    freaky->play(true);
    freaky->setPosition(songpos);
    background = app->assetmanager.get<ImageAsset>(getPath("assets/menu/back.png").c_str());
    selection = 0;
    backgroundy.setValue(-(static_cast<int>(COUNT_OF(menu_selections))/2)*10*selection);

    //initialize menu select sprites
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i] = new Anim_OBJECT("assets/menu/", "menuassets.json");
        menu_selections[i]->setAnim(0, ModeNone);
    }

    option = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/scrollMenu.wav").c_str());
    confirm = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/confirmMenu.wav").c_str());
    back = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/cancelMenu.wav").c_str());
}

void MainMenuScreen::update(void) 
{
    freaky->process();
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i]->tick();
        int frame = i;
        if (selection == i)
            frame += static_cast<int>(COUNT_OF(menu_selections));

        if (!menu_selections[i]->cantick)
            menu_selections[i]->setAnim(frame, ModeNone);
    }
   
    if (app->event.isPressed(Input::MENU_UP))
    {
        app->audioMixer->playBuffer(option->soundbuffer);
        selection -= 1;
        if (selection < 0)
            selection = static_cast<int>(COUNT_OF(menu_selections)-1);

        backgroundy.setValue(-(static_cast<int>(COUNT_OF(menu_selections))/2)*10*selection, BACKGROUND_SPEED);
    }
    else if (app->event.isPressed(Input::MENU_DOWN))    
    {
        app->audioMixer->playBuffer(option->soundbuffer);
        selection += 1;
        if (selection > static_cast<int>(COUNT_OF(menu_selections)-1))
            selection = 0;
        backgroundy.setValue(-(static_cast<int>(COUNT_OF(menu_selections))/2)*10*selection, BACKGROUND_SPEED);
    }
    if (app->event.isPressed(Input::MENU_ENTER))
    {
        app->audioMixer->playBuffer(confirm->soundbuffer);
        switch (selection)
        {
            case 0: //story mode
                setScreen(new StoryModeScreen(freaky->getPosition())); 
                break;
            case 1: //freeplay
                setScreen(new FreeplayScreen(freaky->getPosition())); 
                break;
            case 2: //options
                setScreen(new OptionsScreen(freaky->getPosition())); 
                break;
            case 3: //donate
                setScreen(new DonateScreen(freaky->getPosition())); 
                break;
        }
    }
    if (app->event.isPressed(Input::MENU_ESCAPE))
    {
        app->audioMixer->playBuffer(back->soundbuffer);
        setScreen(new TitleScreen(freaky->getPosition(), Flash)); 
    }
}

void MainMenuScreen::draw(void) 
{
    Gfx::RectWH<int> background_img = {0, 0, 512, 331};
    Gfx::RectWH<int> background_disp = {app->renderer->width/2 - 700/2, backgroundy.getValue(), 700, 397};
    //GFX::drawColTex<float>(&background->image, &background_img, &background_disp, 0, 255, 1, 
                      //      253, 232, 113);

    int y = 0;
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i]->draw(app->renderer->width/2 - menu_selections[i]->frames[menu_selections[i]->curframe].w/2, y + (25 + menu_selections[i]->frames[menu_selections[i]->curframe].h/2), 0, 255, 1);
        y += MENU_SPACING;
    }
}

MainMenuScreen::~MainMenuScreen(void) 
{
    app->assetmanager.release(background->assetpath.c_str()); 
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        delete menu_selections[i];
    }
    delete freaky;
    app->assetmanager.release(option->assetpath.c_str()); 
    app->assetmanager.release(confirm->assetpath.c_str()); 
    app->assetmanager.release(back->assetpath.c_str()); 
}