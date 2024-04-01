#include "playstate.h"
#include <unistd.h>
#include "main.h"
#include "screen.h"
#include "psp/font.h"
#include "psp/pad.h"
#include "chartparser.h"
#include "character.h"

#include "menu/mainmenu.h"

Rating::Rating(std::string name, int score, bool splash, float ratingmod, int hitwindow)
: name(name),
score(score),
noteSplash(splash),
ratingMod(ratingmod),
hitWindow(hitwindow) {}

void PlayStateScreen::initscr(std::string song) {
    setScreenCol(0xFF00FF00);
    //reset vars
    ghosttap = false;
    score = misses = 0;
    cursong = song;
    //init animation
    for (int i = 0; i < 4; i ++)
        notetimer[i] = noteframe[i] = notehit[i] = 0;

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

    sprintf(_path, "assets/songs/%s/Voices.ogg", cursong.c_str());
    if (access(_path, F_OK) == 0) //file exists
    {
        vocals = new Audio::StreamedFile(*app->audioMixer, _path);
        sprintf(_path, "assets/songs/%s/Inst.ogg", cursong.c_str());
        inst = new Audio::StreamedFile(*app->audioMixer, _path);
    }
    else 
    {
        inst = NULL;
        sprintf(_path, "assets/songs/%s/Inst.ogg", cursong.c_str());
        vocals = new Audio::StreamedFile(*app->audioMixer, _path);
    }
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
    ratingData.emplace_back("sick", 350, true,    1,  45);
    ratingData.emplace_back("good", 200, false, 0.7,  90);
    ratingData.emplace_back("bad",  100, false, 0.4, 135);
    ratingData.emplace_back("shit",  50, false,   0, 165);

    app->parser.chartdata.speed /= 5;
}

PlayStateScreen::PlayStateScreen(std::string song)
{
    initscr(song);
}

void PlayStateScreen::Camera::update(float ox, float oy, float oz, float px, float py, float pz) {
    if (app->parser.justStep && !(app->parser.curStep % 16))
    {
        if (app->parser.sections[app->parser.curStep / 16].flag & FLAG_SEC_MUSTHIT) { 
            camx.setValue(px, 0.2);
            camy.setValue(py, 0.2);
            zoom.setValue(pz, 0.3);
        }
        else {
            camx.setValue(ox, 0.2);
            camy.setValue(oy, 0.2);
            zoom.setValue(oz, 0.3);
        }
    }
}

void PlayStateScreen::update(void)
{
    //process audio streams
    if (inst != NULL)
        inst->process();
    vocals->process();

    app->parser.justStep = false;
    app->parser.tickStep(vocals);
    
    bool isPlaying = (vocals->isPlaying());


    if (isPlaying)
    {
        gamecam.update(opponent->camx, opponent->camy, opponent->camzoom,
                      player->camx, player->camy, player->camzoom);
        //bump hud
        if (app->parser.justStep && !(app->parser.curStep % 16)) {
            hudcam.zoom.setValue(1.1, 1.0, 0.2); 
        }
        //bump game
        if (app->parser.justStep && !(app->parser.curStep % 16)) {
            gamecam.zoom.setValue(1.05, 1.0, 0.2); 
        }

        updateInput();
    }
    else
    {
        app->parser.songTime += 15 + app->deltatime; //hardcode to 60fps, should be able to change based on framerate (60fps = 16.666ms)

        //song start
        if (app->parser.curStep <= 0)
        {
            if (app->parser.songTime >= 0 && !isPlaying)
            {
                if (inst != NULL)
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
                //reload the screen
                freescr();
                initscr(nextsong);
            }
            return;
        }
    }
    curstage.tick(gamecam.camx.getValue(), gamecam.camy.getValue());
    //game.player->tick();
//    opponent->tick();
  //  gf->tick();

}

void PlayStateScreen::draw(void)
{
    curstage.drawObjects(curstage.bgobjects, gamecam.zoom.getValue());
//    gf->draw(gamecam.camx, gamecam.camy, gamecam.zoom);
    curstage.drawObjects(curstage.mdobjects, gamecam.zoom.getValue());
  //  opponent->draw(gamecam.camx, gamecam.camy, gamecam.zoom);
    //draw player
    curstage.drawObjects(curstage.fgobjects, gamecam.zoom.getValue());

    drawDummyNotes();
    drawNotes(false);
    drawNotes(true);
  // PrintFont(Left, 0, 40, "mgc %s\nspd%f\nbpm%f\nscnt%d\nncnt%d", app->parser.chartdata.magic, app->parser.chartdata.speed, app->parser.chartdata.bpm, app->parser.chartdata.sectioncount, app->parser.chartdata.notecount);
   // PrintFont(Left, 0, 40, "zoom %f opp %f plr %f", gamecam.zoom, opponent->camzoom, player->camzoom);
   //PrintFont(Left, 0, 40, "note %d, sec %d, data %d", sizeof(Note), sizeof(Section), sizeof(ChartData));
 // PrintFont(Left, 0, 40, "yur rating is: %s  score: %d note: %f, songspeed %f, startnote %d", ratinglol.c_str(), score, (app->parser.gamenotes[3].pos - app->parser.songTime), app->parser.chartdata.speed, startnote);
    PrintFont(Left, 0, 40, "score: %d misses: %d",  score, misses);

}
void PlayStateScreen::freescr(void) {
    delete player;
    delete opponent;
    delete gf;
    curstage.free();
    GFX::freeTex(hud);
    delete inst;
    delete vocals;
}

PlayStateScreen::~PlayStateScreen(void)
{
    freescr();
}

void PlayStateScreen::increaseScore(int note) {
  //  float noteDiff = fabs(app->parser.gamenotes[note].pos - app->parser.songTime);
//    Rating daRating = judgeNote(noteDiff);
}

Rating PlayStateScreen::judgeNote(float diff)
{
    for (int i = 0; i < static_cast<int>(ratingData.size()-1); i++) //skips last window (Shit)
    {
        if (diff <= ratingData[i].hitWindow) {
            return ratingData[i];
        }
    }
    return ratingData[ratingData.size() - 1];
}

void PlayStateScreen::missedNote() {

    if (inst != NULL)
        vocals->setVolume(0,0);
    score -= 10;
    misses += 1;
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
    checkPadHeld[3] = Pad_Held(PSP_CTRL_RIGHT | PSP_CTRL_CIRCLE); 

    //handle note hits here? why not lol
    //opponent
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes[1].size()); i++)
    {
        std::vector<Note> &notes = app->parser.gamenotes[1];
    
        if (notes[i].flag & FLAG_NOTE_HIT)
        {
            deleteNote(i, 1);
            continue;
        }

        int type = notes[i].type;
        int curNotey = ((notes[i].pos - app->parser.songTime) * app->parser.chartdata.speed) + notePos.opponent[type].y;

        //delete note if offscreen
        if (curNotey < -50) {
            if (inst != NULL)
                vocals->setVolume(1,1);
            notes[i].flag |= FLAG_NOTE_HIT;
            deleteNote(i, 1);
            continue;
        }
        else if (curNotey < notePos.opponent[type].y)
        {
            if (inst != NULL)
                vocals->setVolume(1,1);
            notes[i].flag |= FLAG_NOTE_HIT;
        }

        //note is below the screen, so go back to index 0
        if (curNotey > GFX::SCREEN_HEIGHT)
            break;
    }    
    //player
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes[0].size()); i++)
    {
        std::vector<Note> &notes = app->parser.gamenotes[0];
    
        if (notes[i].flag & FLAG_NOTE_HIT)
        {
            deleteNote(i, 0);
            continue;
        }
        int type = notes[i].type;
        int curNotey = ((notes[i].pos - app->parser.songTime) * app->parser.chartdata.speed) + notePos.player[type].y;

        //delete note if offscreen
        if (curNotey < -50) {
            notes[i].flag |= FLAG_NOTE_HIT;
            deleteNote(i, 0);
            missedNote();
            continue;
        }


        float notediff = fabs(notes[i].pos - app->parser.songTime);

        //check if its been hit
        if (checkPad[type] && notediff < static_cast<float>(ratingData[3].hitWindow)) //shit hit window
        {                    
            notehit[type] = true;
            Rating rating = judgeNote(notediff);
            if (inst != NULL)
                vocals->setVolume(1,1);
            notes[i].flag |= FLAG_NOTE_HIT;
            score += rating.score;
            break;
        }
        else if (!ghosttap && (checkPad[0] || checkPad[1] || checkPad[2] || checkPad[3])) //miss note if ghosttapping is off
        {
            //play miss sound todo
            missedNote();
        }
        //note is below the screen, so go back to index 0
        if (curNotey > GFX::SCREEN_HEIGHT)
            break;
    }


/*
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes.size()); i++) {
        if (app->parser.gamenotes[i].flag & FLAG_NOTE_ISOPPONENT || app->parser.gamenotes[i].flag & FLAG_NOTE_HIT)
            continue;

        //hit a note        
        if (checkPadHeld[app->parser.gamenotes[i].type] && 
            app->parser.gamenotes[i].pos-app->parser.songTime >= ratingData[3].hitWindow && //shit hit window
            app->parser.gamenotes[i].pos-app->parser.songTime <= ratingData[0].hitWindow) { //sick hit window

            float noteDiff = app->parser.gamenotes[i].pos - app->parser.songTime;
            app->parser.gamenotes[i].flag |= FLAG_NOTE_HIT; //note has been hit
        
            Rating daRating = judgeNote(noteDiff);
            score += daRating.score;
            ratinglol = daRating.name;

        }
        //missed a note
        //else if (checkPad[app->parser.gamenotes[i].type] || ((app->parser.gamenotes[i].pos - app->parser.songTime) <= 0)) {
        //    missedNote();
        //    if ((app->parser.gamenotes[i].pos - app->parser.songTime) <= 0)
        //        app->parser.gamenotes[i].flag |= FLAG_NOTE_HIT; //note has been hit (lol no but im doing this to ignore the note)
        //
        //}
        else continue;
    }*/
}
