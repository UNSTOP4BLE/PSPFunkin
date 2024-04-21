#include "mainmenu.h"
#include "../main.h"
#include "../app.h"

#include "storymode.h"
#include "freeplay.h"
#include "options.h"
#include "donate.h"

#define MENU_SPACING 50
#define BACKGROUND_SPEED 0.2

MainMenuScreen::MainMenuScreen(void) {
    background = new GFX::Texture();
    background->load(getPath("assets/menu/back.png").c_str());
    selection = 0;
    backgroundy.setValue(-(static_cast<int>(COUNT_OF(menu_selections))/2)*10*selection);

    //initialize menu select sprites
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i] = new Anim_OBJECT(getPath("assets/menu/").c_str(), "menuassets.json");
        menu_selections[i]->setAnim(0, ModeNone);
    }
}

void MainMenuScreen::update(void) 
{
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
        selection -= 1;
        if (selection < 0)
            selection = 0;

        backgroundy.setValue(-(static_cast<int>(COUNT_OF(menu_selections))/2)*10*selection, BACKGROUND_SPEED);
    }
    else if (app->event.isPressed(Input::MENU_DOWN))    
    {
        selection += 1;
        if (selection > static_cast<int>(COUNT_OF(menu_selections)-1))
            selection = static_cast<int>(COUNT_OF(menu_selections)-1);
        backgroundy.setValue(-(static_cast<int>(COUNT_OF(menu_selections))/2)*10*selection, BACKGROUND_SPEED);
    }
    if (app->event.isPressed(Input::MENU_ENTER))
    {
        switch (selection)
        {
            case 0: //story mode
                setScreen(new StoryModeScreen()); 
                break;
            case 1: //freeplay
                setScreen(new FreeplayScreen()); 
                break;
            case 2: //options
                setScreen(new OptionsScreen()); 
                break;
            case 3: //donate
                setScreen(new DonateScreen()); 
                break;
        }
    }
}

void MainMenuScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {GFX::SCREEN_WIDTH/2 - 700/2, backgroundy.getValue(), 700, 397};
    GFX::drawTex<float>(background, &background_img, &background_disp, 0, 255, 1);

    int y = 0;
    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        menu_selections[i]->draw(GFX::SCREEN_WIDTH/2 - menu_selections[i]->frames[menu_selections[i]->curframe].w/2, y + (25 + menu_selections[i]->frames[menu_selections[i]->curframe].h/2), 0, 255, 1);
        y += MENU_SPACING;
    }
}

MainMenuScreen::~MainMenuScreen(void) 
{
    delete background;

    for (int i = 0; i < static_cast<int>(COUNT_OF(menu_selections)); i++)
    {
        delete menu_selections[i];
    }
}