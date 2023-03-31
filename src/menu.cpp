#include "main.h"
#include "screen.h"
#include "psp/font.h"
#include "chartparser.h"
#include "psp/animation.h"
#include "psp/audio_file_readers.h"
#include "psp/pad.h"

enum TitleStates
{
    Intro,
    Flash,
    Title
};
TitleStates state;

void TitleScreen::load(void) 
{
    setScreenCol(BLACK);

    //load menu jsons
    Json::Value titleJson;
    loadJson("assets/menu/title/title.json", &titleJson);
    
    int curmsg = rand() % (titleJson["messages"].size()-1); //get a random message
    funnymessage[0] = titleJson["messages"][curmsg][0].asString();
    funnymessage[1] = titleJson["messages"][curmsg][1].asString();

    //load and play music
    //menutrack = Mix_LoadMUS("assets/songs/freaky/freaky.wav");
    //Mix_PlayMusic(menutrack, true);
    parser.initbpm = titleJson["menuSongBPM"].asDouble();   
    Parser_calcCrochet();
    freaky = new Audio::StreamedFile("assets/songs/freaky/freaky.ogg");
    freaky->play(true);
    confirm = Audio::loadFile("assets/sounds/confirmMenu.ogg");

    //load textures
    AnimOBJECT_Init(&titleGF, "assets/menu/title/gf/", "frames.json");
    ng = g2dTexLoad("assets/menu/title/ng.png", G2D_SWIZZLE);

    //begin with a flash if you went back to title from menu
    if (state != Intro)
        state = Flash;
}

void TitleScreen::update(void) 
{
    //process audio streams
    freaky->process();

    parser.justStep = false;
    Parser_tickStep(freaky);
    Bold_Tick(); //animate bold font

    if (parser.justStep && !(parser.curStep % 4))
    {
        gfBop = !gfBop;
        AnimOBJECT_SetAnim(&titleGF, gfBop);
    }
    AnimOBJECT_Tick(&titleGF);
    if (Pad_Pressed(PSP_CTRL_CROSS | PSP_CTRL_START))
    {
        if (state != Title)
            state = Title;
        else
            setScreen(new PlayStateScreen());
    }
}

void TitleScreen::draw(void) 
{
    PrintFont(Left, 0, 0, "%d", parser.songPos);
    //NG logo 
    Rect NG_img = {0, 0, 90, 88};
    Rect NG_disp = {G2D_SCR_W / 2 - 90/2, (G2D_SCR_H / 2 + 88/2) - 20, 90, 88};
    switch (state)
    {
        case Intro:
            switch (parser.curBeat) 
            {
                case 3:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 + 76, "PRESENT");
                case 2:
                case 1:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 114, "UNSTOPABLE");
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 76,  "IGORSOU");
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 38,  "MAXDEV");
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2,       "SPICYJPEG");
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 + 38,  "BILIOUS");
                    break;
                case 7:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 20, "NEWGROUNDS");
                    DrawG2DTex(ng, &NG_img, &NG_disp, false, 0, 255);
                case 6:
                case 5:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 96, "IN ASSOCIATION");
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 58, "WITH");
                    break;

                case 11:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, funnymessage[1].c_str());
                case 10:
                case 9:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 38, funnymessage[0].c_str());
                    break;
                case 15:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 + 18, "FUNKIN");
                case 14:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 20, "NIGHT");
                case 13:
                    PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 58, "FRIDAY");
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
            AnimOBJECT_Draw(&titleGF, 200, 200);
            break;
    }
}

void TitleScreen::deload(void) 
{
    delete freaky;
    g2dTexFree(&ng);
}