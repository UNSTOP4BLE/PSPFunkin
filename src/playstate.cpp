#include "main.h"
#include "screen.h"
#include "psp/font.h"
#include "psp/pad.h"
#include "chartparser.h"
#include "character.h"

const char *song = "bopeebo";

PlayStateScreen::PlayStateScreen(void)
{
    setScreenCol(GREEN);

    char _path[40];
    sprintf(_path, "assets/songs/%s/config.json", song);
    Json::Value _config;
    loadJson(_path, &_config);

    //load characters
//    sprintf(_path, "assets/characters/%s/", _config["player"].asString().c_str());
    sprintf(_path, "assets/characters/%s/", _config["opponent"].asString().c_str());
    player = new Character(_path, _config["opponent"].asString() + ".json", _config["playerpos"][0].asFloat(), _config["playerpos"][1].asFloat());
    player->setFocus(_config["playerpos"][2].asFloat(), _config["playerpos"][3].asFloat(),  _config["playerpos"][4].asFloat());

    //player = new Character(_path);
    //opponent
    sprintf(_path, "assets/characters/%s/", _config["opponent"].asString().c_str());
    opponent = new Character(_path, _config["opponent"].asString() + ".json", _config["opponentpos"][0].asFloat(), _config["opponentpos"][1].asFloat());
    opponent->setFocus(_config["opponentpos"][2].asFloat(), _config["opponentpos"][3].asFloat(),  _config["opponentpos"][4].asFloat());
    //gf    
    sprintf(_path, "assets/characters/%s/", _config["gf"].asString().c_str());
    gf = new Character(_path, _config["gf"].asString() + ".json", _config["gfpos"][0].asFloat(), _config["gfpos"][1].asFloat());

    //load game assets
    sprintf(_path, "assets/songs/%s/%s.bin", song, song); //todo implement difficulty
    app->parser.loadChart(_path);
    app->parser.songTime = -3000;

    sprintf(_path, "assets/songs/%s/Inst.ogg", song);
    inst = new Audio::StreamedFile(*app->audioMixer, _path);
    sprintf(_path, "assets/songs/%s/Voices.ogg", song);
    vocals = new Audio::StreamedFile(*app->audioMixer, _path);

    hud = g2dTexLoad("assets/hud.png", G2D_SWIZZLE);
    camera.camx = 0;
    camera.camy = 0;
    camera.zoom = 1; 
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

void PlayStateScreen::Camera::update(float ox, float oy, float oz, float px, float py, float pz) {
    if (app->parser.sections[app->parser.curStep / 16].flag & FLAG_SEC_MUSTHIT) { 
        camx = app->lerp(camx, px, 0.2);
        camy = app->lerp(camy, py, 0.2);
        zoom = app->lerp(zoom, pz, 0.3);
    }
    else {
        camx = app->lerp(camx, ox, 0.2);
        camy = app->lerp(camy, oy, 0.2);
        zoom = app->lerp(zoom, oz, 0.3);
    }
}

void PlayStateScreen::update(void)
{
    //process audio streams
    inst->process();
    vocals->process();

    app->parser.justStep = false;
    app->parser.tickStep(vocals);
    bool isPlaying = (inst->isPlaying() || vocals->isPlaying());

    if (isPlaying)
    {
        camera.update(opponent->camx, opponent->camy, opponent->camzoom,
                      player->camx, player->camy, player->camzoom);
        updateInput();
    }
    else
    {
        app->parser.songTime += 16 + app->deltatime;

        //song start
        if (app->parser.curStep <= 0)
        {
            if (app->parser.songTime >= 0 && !isPlaying)
            {
                inst->play();
                vocals->play();
            }
        }
        else
        {
            setScreen(new MainMenuScreen());
        }
    }

    //game.player->tick();
    opponent->tick();
    gf->tick();

}

void PlayStateScreen::draw(void)
{
    opponent->draw(camera.camx, camera.camy, camera.zoom);
    gf->draw(camera.camx, camera.camy, camera.zoom);

    drawDummyNotes();
    drawNotes();
  // PrintFont(Left, 0, 40, "mgc %s\nspd%f\nbpm%f\nscnt%d\nncnt%d", app->parser.chartdata.magic, app->parser.chartdata.speed, app->parser.chartdata.bpm, app->parser.chartdata.sectioncount, app->parser.chartdata.notecount);
    PrintFont(Left, 0, 40, "zoom %f opp %f plr %f", camera.zoom, opponent->camzoom, player->camzoom);
   //PrintFont(Left, 0, 40, "note %d, sec %d, data %d", sizeof(Note), sizeof(Section), sizeof(ChartData));


}
PlayStateScreen::~PlayStateScreen(void)
{
    g2dTexFree(&hud);
    delete inst;
    delete vocals;
    delete player;
    delete opponent;
    delete gf;
}

void PlayStateScreen::updateInput(void)
{
    checkPad[0] = Pad_Pressed(PSP_CTRL_LEFT | PSP_CTRL_SQUARE);
    checkPad[1] = Pad_Pressed(PSP_CTRL_DOWN | PSP_CTRL_CROSS | PSP_CTRL_LTRIGGER);
    checkPad[2] = Pad_Pressed(PSP_CTRL_UP | PSP_CTRL_TRIANGLE | PSP_CTRL_RTRIGGER);
    checkPad[3] = Pad_Pressed(PSP_CTRL_RIGHT | PSP_CTRL_CIRCLE);
    checkPadHeld[0] = Pad_Held(PSP_CTRL_LEFT | PSP_CTRL_SQUARE);
    checkPadHeld[1] = Pad_Held(PSP_CTRL_DOWN | PSP_CTRL_CROSS | PSP_CTRL_LTRIGGER);
    checkPadHeld[2] = Pad_Held(PSP_CTRL_UP | PSP_CTRL_TRIANGLE | PSP_CTRL_RTRIGGER);
    checkPadHeld[3] = Pad_Held(PSP_CTRL_LEFT | PSP_CTRL_CIRCLE); 

    //handle note hits here? why not lol
    for (int i = 0; i < (int)app->parser.gamenotes.size(); i++) {
        if (app->parser.gamenotes[i].flag & FLAG_NOTE_ISOPPONENT)
            continue;
        int y = notePos.player[app->parser.gamenotes[i].type].y;
        int hitwindows[4][2] = { //[amount of ratings], [min, max]
            {y-64, y-48}, //shit
            {y-48, y-32}, //bad 
            {y-32, y-16}, //good
            {y-16, y+16}  //sick
        };

        float curNotey = ((app->parser.gamenotes[i].pos - app->parser.songTime) * app->parser.chartdata.speed / 3.6f) + y;

        if (checkPad[app->parser.gamenotes[i].type] && curNotey >= hitwindows[0][0] && curNotey <= hitwindows[3][1]) {
            app->parser.gamenotes[i].flag |= FLAG_NOTE_HIT; //note has been hit
            for (int j = 0; j < 4; j++) {
                if (curNotey >= hitwindows[j][0] && curNotey <= hitwindows[j][1]) {
                    //j = rating;
                    break;
                }
            }
        }

    }
}