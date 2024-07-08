#include "title.h"
#include "../app.h"
#include "../main.h"
#include "../screen.h"
#include "../psp/font.h"
#include "../chartparser.h"
#include "../psp/animation.h"
#include "../psp/audio_file_readers.h"
#include "../psp/input.h"
#include <chrono>

#include "mainmenu.h"

TitleScreen::TitleScreen(int songpos, TitleStates _state) 
{
    setScreenCol(0xFF000000);

    //load menu jsons
    const JsonAsset *titleJson = app->assetmanager.get<JsonAsset>(getPath("assets/menu/title/title.json").c_str());
    
    int curmsg = rand() % (titleJson->value["messages"].size()); //get a random message
    funnymessage[0] = titleJson->value["messages"][curmsg][0].asString();
    funnymessage[1] = titleJson->value["messages"][curmsg][1].asString();


    //load and play music
    app->parser.chartdata.bpm = titleJson->value["menuSongBPM"].asDouble();   
    app->parser.calcCrochet();
    freaky = new Audio::StreamedFile(*app->audioMixer, getPath("assets/music/freaky/freaky.ogg").c_str());
    freaky->play(true);
    freaky->setPosition(songpos);
    confirm = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/confirmMenu.wav").c_str());
    //load textures
    titleGF = new Anim_OBJECT("assets/menu/title/gf/", "frames.json");
    titleGF->setAnim(0, ModeNone);
    logo = new Anim_OBJECT("assets/menu/title/", "logoBumpin.json");
    logo->setAnim(0, ModeNone);
    ng = app->assetmanager.get<ImageAsset>(getPath("assets/menu/title/ng.png").c_str());

    app->assetmanager.release(titleJson->assetpath.c_str());
    state = _state;
    //begin with a flash if you went back to title from menu
    if (state != Intro)
        state = Flash;
}

void TitleScreen::update(void) 
{
    //process audio streams
    freaky->process();

    app->parser.justStep = false;
    app->parser.tickStep(freaky);

    if (app->parser.justStep && !(app->parser.curStep % 4))
    {
        logo->setAnim(0, ModeStep);

        if (titleGF->curanim.anim == 1)
            titleGF->setAnim(0, ModeStep);
        else
            titleGF->setAnim(1, ModeStep);
    }
    logo->tick();
    titleGF->tick();
    if (app->event.isPressed(Input::MENU_ENTER))
    {
        if (state != Title)
            state = Title;
        else
        {        
            app->audioMixer->playBuffer(confirm->soundbuffer);
            setScreen(new MainMenuScreen(freaky->getPosition()));
        }
    }
}

void TitleScreen::draw(void) 
{
    //NG logo 
    GFX::RECT<int> NG_img = {0, 0, 90, 88};
    GFX::RECT<int> NG_disp = {app->screenwidth / 2 - 90/2, (app->screenheight / 2 + 88/2) - 20, 90, 88};
    switch (state)
    {
        case Intro:
            switch (app->parser.curBeat) 
            {
                case 3:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 + 38, "PRESENT");
                        [[fallthrough]];
                case 2:
                        [[fallthrough]];
                case 1:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 57, "UNSTOPABLE");
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 38,  "IGORSOU");
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 19,  "MAXDEV");
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2,       "SPICYJPEG");
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 + 19,  "BILIOUS");
                    break;
                case 7:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 10, "NEWGROUNDS");
                    GFX::drawTex<int>(&ng->image, &NG_img, &NG_disp, 0, 255, 1);
                        [[fallthrough]];
                case 6:
                        [[fallthrough]];
                case 5:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 48, "IN ASSOCIATION");
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 29, "WITH");
                    break;

                case 11:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2, funnymessage[1].c_str());
                        [[fallthrough]];
                case 10:
                        [[fallthrough]];
                case 9:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 19, funnymessage[0].c_str());
                    break;
                case 15:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 + 9, "FUNKIN");
                        [[fallthrough]];
                case 14:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 10, "NIGHT");
                        [[fallthrough]];
                case 13:
                    app->boldFont->Print(Center, app->screenwidth / 2, app->screenheight / 2 - 29, "FRIDAY");
                    break;
                case 16:
                    state = Flash;
                    break;
            }
            break;
        case Flash:
            //draw flash
            state = Title;
            break;
        case Title:
            titleGF->draw(320, 228, 0, 255, 1.4);
            logo->draw(40, 30, 0, 255, 1.4);
            break;
    }
}

TitleScreen::~TitleScreen(void) 
{
    app->assetmanager.release(ng->assetpath.c_str()); 
    delete titleGF;
    delete logo;
    delete freaky;
    app->assetmanager.release(confirm->assetpath.c_str()); 
}