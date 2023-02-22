#include "screen.h"
#include "psp/font.h"
#include "psp/pad.h"
#include "chartparser.h"
const char *song = "frostbite";
#include "charlist.h"

void PlayStateScreen::load(void)
{
    setScreenCol(GREEN);

    char _path[40];
    sprintf(_path, "assets/songs/%s/config.json", song);
    Json::Value _config;
    loadJson(_path, &_config);

    //load characters
    //setChar(_config["player"].asString());
    //setChar(_config["opponent"].asString());
   // setChar(_config["gf"].asString());
    //game.player->setAnim(IDLE);
//  PlayStateScreen::opponent->setAnim(IDLE);
//  PlayStateScreen::gf->setAnim(IDLE);

    //load game assets
    sprintf(_path, "assets/songs/%s/%s.json", song, song); //todo implement difficulty
    Parser_loadChart(_path);
    Parser_readInitialData();
    Parser_readChartData(chartData);
    parser.songPos = -3000;

    //sprintf(_path, "assets/songs/%s/Inst.wav", song);
    //inst = Audio_LoadSong(_path);
    sprintf(_path, "assets/songs/%s/Voices.wav", song);
    PlayStateScreen::vocals = Mix_LoadMUS(_path);

    hud = g2dTexLoad("assets/hud.png", G2D_SWIZZLE);

    //set note positions
    notePos.player[0] = {294,  14}; 
    notePos.player[1] = {334,  14}; 
    notePos.player[2] = {374,  14}; 
    notePos.player[3] = {414,  14}; 
    notePos.opponent[0] = { 27,  14};   
    notePos.opponent[1] = { 67,  14};   
    notePos.opponent[2] = {107,  14};   
    notePos.opponent[3] = {147,  14};   
}
void PlayStateScreen::update(void)
{
    parser.justStep = false;
    Parser_tickStep(PlayStateScreen::vocals);

    if (Mix_PlayingMusic())
    {
        updateInput();
    }
    else
    {
        parser.songPos += 16 + getDT();

        //song start
        if (parser.curStep <= 0)
        {
            if (parser.songPos >= 0 && !Mix_PlayingMusic())
            {
                //Mix_PlayMusic(PlayStateScreen::inst, false);
                Mix_PlayMusic(PlayStateScreen::vocals, false);
            }

        }
    }
   
    PrintFont(Left, 0, 0, "step %d time %d", parser.curStep, parser.songPos);

    //game.player->tick();
//  PlayStateScreen::opponent->tick();
//  PlayStateScreen::gf->tick();

}

void PlayStateScreen::draw(void)
{
    drawDummyNotes();
    drawNotes();
}

void PlayStateScreen::deload(void)
{
}

void PlayStateScreen::updateInput(void)
{
    checkPad[0] = Pad_Pressed(PSP_CTRL_LEFT | PSP_CTRL_SQUARE);
    checkPad[1] = Pad_Pressed(PSP_CTRL_DOWN | PSP_CTRL_CROSS | PSP_CTRL_LTRIGGER);
    checkPad[2] = Pad_Pressed(PSP_CTRL_UP | PSP_CTRL_TRIANGLE | PSP_CTRL_RTRIGGER);
    checkPad[3] = Pad_Pressed(PSP_CTRL_LEFT | PSP_CTRL_CIRCLE);
    checkPadHeld[0] = Pad_Pressed(PSP_CTRL_LEFT | PSP_CTRL_SQUARE);
    checkPadHeld[1] = Pad_Pressed(PSP_CTRL_DOWN | PSP_CTRL_CROSS | PSP_CTRL_LTRIGGER);
    checkPadHeld[2] = Pad_Pressed(PSP_CTRL_UP | PSP_CTRL_TRIANGLE | PSP_CTRL_RTRIGGER);
    checkPadHeld[3] = Pad_Pressed(PSP_CTRL_LEFT | PSP_CTRL_CIRCLE); 
}