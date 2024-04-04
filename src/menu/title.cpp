#include "title.h"
#include "../app.h"
#include "../main.h"
#include "../screen.h"
#include "../psp/font.h"
#include "../chartparser.h"
#include "../psp/animation.h"
#include "../psp/audio_file_readers.h"
#include "../psp/pad.h"
#include <chrono>

#include "mainmenu.h"

enum TitleStates
{
    Intro,
    Flash,
    Title
};
TitleStates state;

TitleScreen::TitleScreen(void) 
{
    setScreenCol(0xFF000000);

    //load menu jsons
    Json::Value titleJson;
    loadJson("assets/menu/title/title.json", &titleJson);
    
    int curmsg = rand() % (titleJson["messages"].size()-1); //get a random message
    funnymessage[0] = titleJson["messages"][curmsg][0].asString();
    funnymessage[1] = titleJson["messages"][curmsg][1].asString();

    //load and play music
    app->parser.chartdata.bpm = titleJson["menuSongBPM"].asDouble();   
    app->parser.calcCrochet();
    freaky = new Audio::StreamedFile(*app->audioMixer, "assets/songs/freaky/freaky.ogg");
    freaky->play(true);
    // confirm = Audio::loadFile("assets/songs/stream.vag");
   // app->audioMixer->playBuffer(*confirm);
    //load textures
//    AnimOBJECT_Init(&titleGF, "assets/menu/title/gf/", "frames.json");
    ng = GFX::loadTex("assets/menu/title/ng.png");

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
    Bold_Tick(); //animate bold font

    if (app->parser.justStep && !(app->parser.curStep % 4))
    {
        //if (titleGF.curanim == 1)
   //         AnimOBJECT_SetAnim(&titleGF, 0, -1);
    //    else
      //      AnimOBJECT_SetAnim(&titleGF, 1, -1);
    }
//    AnimOBJECT_Tick(&titleGF);
    if (Pad_Pressed(PSP_CTRL_CROSS | PSP_CTRL_START))
    {
        if (state != Title)
            state = Title;
        else
            setScreen(new MainMenuScreen());
    }
}

void TitleScreen::draw(void) 
{
    if (freaky->isPlaying())
    PrintFont(Left, 0, 0, "time %d", app->parser.songTime);
    //NG logo 
    GFX::RECT<int> NG_img = {0, 0, 90, 88};
    GFX::RECT<int> NG_disp = {GFX::SCREEN_WIDTH / 2 - 90/2, (GFX::SCREEN_HEIGHT / 2 + 88/2) - 20, 90, 88};
    switch (state)
    {
        case Intro:
            switch (app->parser.curBeat) 
            {
                case 3:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 + 76, "PRESENT");
                        [[fallthrough]];
                case 2:
                        [[fallthrough]];
                case 1:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 114, "UNSTOPABLE");
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 76,  "IGORSOU");
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 38,  "MAXDEV");
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2,       "SPICYJPEG");
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 + 38,  "BILIOUS");
                    break;
                case 7:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 20, "NEWGROUNDS");
                    GFX::drawTex<int>(ng, &NG_img, &NG_disp, 0, 255);
                        [[fallthrough]];
                case 6:
                        [[fallthrough]];
                case 5:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 96, "IN ASSOCIATION");
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 58, "WITH");
                    break;

                case 11:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2, funnymessage[1].c_str());
                        [[fallthrough]];
                case 10:
                        [[fallthrough]];
                case 9:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 38, funnymessage[0].c_str());
                    break;
                case 15:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 + 18, "FUNKIN");
                        [[fallthrough]];
                case 14:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 20, "NIGHT");
                        [[fallthrough]];
                case 13:
                    PrintBOLD(Center, GFX::SCREEN_WIDTH / 2, GFX::SCREEN_HEIGHT / 2 - 58, "FRIDAY");
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
          //  AnimOBJECT_Draw(&titleGF, 200, 200, 0, 255, 1.0f);
            break;
    }
}

TitleScreen::~TitleScreen(void) 
{
    delete freaky;
    GFX::freeTex(ng);
}