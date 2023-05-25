#include "main.h"
#include "screen.h"
#include "psp/font.h"
#include "psp/pad.h"
#include "chartparser.h"
#include "character.h"

static void setRating(Rating &rating, std::string name, int score, bool splash, float ratingmod, int hitwindow) {
    rating.name = name;
    rating.score = score;
    rating.noteSplash = splash;
    rating.ratingMod = ratingmod;
    rating.hitWindow = hitwindow;
}

void PlayStateScreen::initscr(void) {
    setScreenCol(0xFF00FF00);

    cursong = "bopeebo";
    char _path[40];
    sprintf(_path, "assets/songs/%s/config.json", cursong.c_str());
    Json::Value _config;
    loadJson(_path, &_config);

    //next song info
    nextsong = _config["next"].asString();

    //load characters

    //player
    sprintf(_path, "assets/characters/%s/", _config["opponent"].asString().c_str());
    player = new Character(_path, _config["opponent"].asString() + ".json", _config["playerpos"][0].asFloat(), _config["playerpos"][1].asFloat());
    player->setFocus(_config["playerpos"][2].asFloat(), _config["playerpos"][3].asFloat(),  _config["playerpos"][4].asFloat());

    //opponent
    sprintf(_path, "assets/characters/%s/", _config["opponent"].asString().c_str());
    opponent = new Character(_path, _config["opponent"].asString() + ".json", _config["opponentpos"][0].asFloat(), _config["opponentpos"][1].asFloat());
    opponent->setFocus(_config["opponentpos"][2].asFloat(), _config["opponentpos"][3].asFloat(),  _config["opponentpos"][4].asFloat());

    //gf    
    sprintf(_path, "assets/characters/%s/", _config["gf"].asString().c_str());
    gf = new Character(_path, _config["gf"].asString() + ".json", _config["gfpos"][0].asFloat(), _config["gfpos"][1].asFloat());

    //stage
    sprintf(_path, "assets/stages/%s/%s.json", _config["back"].asString().c_str(), _config["back"].asString().c_str()); 
    curstage.load(_path, _config["back"].asString());

    //load game assets
    sprintf(_path, "assets/songs/%s/%s.bin", cursong.c_str(), cursong.c_str()); //todo implement difficulty
    app->parser.loadChart(_path);
    app->parser.songTime = -3000;

    sprintf(_path, "assets/songs/%s/Inst.ogg", cursong.c_str());
    inst = new Audio::StreamedFile(*app->audioMixer, _path);
    sprintf(_path, "assets/songs/%s/Voices.ogg", cursong.c_str());
    vocals = new Audio::StreamedFile(*app->audioMixer, _path);

    hud = GFX::loadTex("assets/hud.png");
    gamecam.camx = 0;
    gamecam.camy = 0;
    gamecam.zoom = 1;
    hudcam.camx = 0;
    hudcam.camy = 0;
    hudcam.zoom = 1;

    //set note positions
    notePos.player[0] = {294,  14}; 
    notePos.player[1] = {334,  14}; 
    notePos.player[2] = {374,  14}; 
    notePos.player[3] = {414,  14}; 
    notePos.opponent[0] = { 27,  14};   
    notePos.opponent[1] = { 67,  14};   
    notePos.opponent[2] = {107,  14};   
    notePos.opponent[3] = {147,  14};   

    //set up ratings and timings (kinda stolen off of psych engine lmao)
    //setRating(Rating &rating, std::string name, int score, bool splash, float ratingmod, int hitwindow)
    Rating rating;
    setRating(rating, "sick", 350, true,    1,  45);
    ratingData.push_back(rating); //default rating
    setRating(rating, "good", 200, false, 0.7,  90); 
    ratingData.push_back(rating);
    setRating(rating, "bad",  100, false, 0.4, 135); 
    ratingData.push_back(rating);
    setRating(rating, "shit",  50, false,   0,   0); 
    ratingData.push_back(rating);
}

PlayStateScreen::PlayStateScreen(void)
{
    initscr();
}

void PlayStateScreen::Camera::update(float ox, float oy, float oz, float px, float py, float pz) {
    if (app->parser.sections[app->parser.curStep / 16].flag & FLAG_SEC_MUSTHIT) { 
        camx = lerp(camx, px, 0.2);
        camy = lerp(camy, py, 0.2);
        zoom = lerp(zoom, pz, 0.3);
    }
    else {
        camx = lerp(camx, ox, 0.2);
        camy = lerp(camy, oy, 0.2);
        zoom = lerp(zoom, oz, 0.3);
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
        gamecam.update(opponent->camx, opponent->camy, opponent->camzoom,
                      player->camx, player->camy, player->camzoom);
        //bump hud
        hudcam.zoom = lerp(hudcam.zoom, 1.0, 0.2); 
        if (app->parser.justStep && !(app->parser.curStep % 16))
            hudcam.zoom = 1.1; 
        //bump game
        hudcam.zoom = lerp(hudcam.zoom, 1.0, 0.2); 
        if (app->parser.justStep && !(app->parser.curStep % 16))
            hudcam.zoom = 1.05;

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
            if (nextsong == "NULL" || nextsong == "null") {
                setScreen(new MainMenuScreen());
            }
            else {
                cursong = nextsong;
                //reload the screen
                freescr();
                initscr();
            }
            return;
        }
    }
    curstage.tick(gamecam.camx, gamecam.camy);
    //game.player->tick();
//    opponent->tick();
  //  gf->tick();

}

void PlayStateScreen::draw(void)
{
    curstage.drawObjects(curstage.bgobjects, gamecam.zoom);
//    gf->draw(gamecam.camx, gamecam.camy, gamecam.zoom);
    curstage.drawObjects(curstage.mdobjects, gamecam.zoom);
  //  opponent->draw(gamecam.camx, gamecam.camy, gamecam.zoom);
    //draw player
    curstage.drawObjects(curstage.fgobjects, gamecam.zoom);

    drawDummyNotes();
    drawNotes();
  // PrintFont(Left, 0, 40, "mgc %s\nspd%f\nbpm%f\nscnt%d\nncnt%d", app->parser.chartdata.magic, app->parser.chartdata.speed, app->parser.chartdata.bpm, app->parser.chartdata.sectioncount, app->parser.chartdata.notecount);
   // PrintFont(Left, 0, 40, "zoom %f opp %f plr %f", gamecam.zoom, opponent->camzoom, player->camzoom);
   //PrintFont(Left, 0, 40, "note %d, sec %d, data %d", sizeof(Note), sizeof(Section), sizeof(ChartData));


}

void PlayStateScreen::freescr(void) {
    delete player;
    delete opponent;
    delete gf;
    curstage.free();
    GFX::freeTex(&hud);
    delete inst;
    delete vocals;
}

PlayStateScreen::~PlayStateScreen(void)
{
    freescr();
}

void PlayStateScreen::increaseScore(int note) {
    float noteDiff = fabs(app->parser.gamenotes[note].pos - app->parser.songTime);
    Rating daRating = judgeNote(noteDiff);
}

Rating PlayStateScreen::judgeNote(float diff)
{
    for (int i = 0; i < (int)ratingData.size()-1; i++) //skips last window (Shit)
    {
        if (diff <= ratingData[i].hitWindow)
        {
            return ratingData[i];
        }
    }
    return ratingData[ratingData.size() - 1];
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
        float curNotey = ((app->parser.gamenotes[i].pos - app->parser.songTime) * app->parser.chartdata.speed / 3.6f) + y;

     //   if (checkPad[app->parser.gamenotes[i].type] && curNotey >= hitwindows[0][0] && curNotey <= hitwindows[3][1]) {
      //      app->parser.gamenotes[i].flag |= FLAG_NOTE_HIT; //note has been hit
        //    for (int j = 0; j < 4; j++) {
          //      if (curNotey >= hitwindows[j][0] && curNotey <= hitwindows[j][1]) {
            //        //j = rating;
              //      break;
           //     }
            //}
     //   }

    }
}