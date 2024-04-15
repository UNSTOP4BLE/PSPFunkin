#include "freeplay.h"
#include "../app.h"
#include <dirent.h>
#include <stdio.h>
#include <algorithm>
#include "../psp/font.h"

#include "mainmenu.h"
#include "../playstate.h"

FreeplayScreen::FreeplayScreen(void) {
    background = new GFX::Texture();
    background->load(getPath("assets/menu/back.png").c_str());
    selection = 0;
    texty.setValue(((selection)*20));

    const char* songsPath = getPath("assets/songs").c_str();

    DIR *dir = opendir(songsPath);

    for (dirent *entry = readdir(dir); entry != NULL; entry = readdir(dir))
    {
        //todo filter out . and .. 
        std::string name(entry->d_name);
        if (name == "." || name == "..") 
            continue;
        for (auto& x : name)
            x = std::toupper(x); 

        printf("song found(%s)\n", name.c_str());
        songs.push_back(name);
    }

    closedir(dir);
}

void FreeplayScreen::update(void) 
{
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
        texty.setValue(((selection)*20), 0.2);
    }
    else if (app->event.isPressed(Input::MENU_DOWN))    
    {
        selection += 1;
        if (selection > static_cast<int>(songs.size()-1))
            selection = static_cast<int>(songs.size()-1);
        texty.setValue(((selection)*20), 0.2);
    }
    if (app->event.isPressed(Input::MENU_ENTER))
    {
        setScreen(new PlayStateScreen(songs[selection], true));
    }

    if (app->event.isPressed(Input::MENU_ESCAPE))
    {
        setScreen(new MainMenuScreen());
    }
}

void FreeplayScreen::draw(void) 
{
    GFX::RECT<int> background_img = {0, 0, 512, 331};
    GFX::RECT<float> background_disp = {GFX::SCREEN_WIDTH/2 - 700/2, GFX::SCREEN_HEIGHT/2 - 397/2, 700, 397};
    GFX::drawTex<float>(background, &background_img, &background_disp, 0, 255);

    for (int i = 0; i < static_cast<int>(songs.size()); i++)
    {
        app->boldFont->setZoom(1.5);
        int alpha = 128;
        if (selection == i) //set alpha to full if song is selected
            alpha = 255;
        app->boldFont->setAlpha(alpha);

        //replace - with space when drawing song
        std::string cursong = songs[i];
        std::replace(cursong.begin(), cursong.end(), '-', ' '); 

        app->boldFont->Print(Left, static_cast<int>((50+((i-selection)*2)) * 1.5), static_cast<int>((60 + -texty.getValue() + (i*20)) * 1.5), "%s", cursong.c_str());
    }
    app->normalFont->Print(Left, 0, 20, "%s", songs[selection].c_str());
}

FreeplayScreen::~FreeplayScreen(void) 
{
    delete background;
}