#include "playstate.h"
#include <unistd.h>
#include "main.h"
#include "psp/hash.h"
#include "screen.h"
#include "psp/font.h"
#include "psp/input.h"
#include "chartparser.h"
#include "character.h"
#include "app.h"

#include "menu/freeplay.h"
#include "menu/storymode.h"

#define CAMERA_ZOOM_OFFSET 0.3
Rating::Rating(std::string name, int hitwindow, float ratingmod, int score, bool splash)
: name(name),
hitWindow(hitwindow),
ratingMod(ratingmod),
score(score),
noteSplash(splash) {}

void PlayStateScreen::initscr(std::string song, bool _freeplay) {
    app->timer.start();
    //reset vars
    bumpfreq = 16;
    isfreeplay = _freeplay;
    combo.init();
    ghosttap = true;
    botplay = false;
    score = misses = 0;
    cursong = song;
    health = 0.5;
    countdown_alpha.setValue(0);
    countdown_img = {311, 205, 200, 90};
    countdown_disp = {(app->renderer->width/2) - (countdown_img.w/2), (app->renderer->height/2) - (countdown_img.h/2), countdown_img.w, countdown_img.h};

    //init animation
    for (int i = 0; i < 4; i ++)
    {
        noteframe[i].setValue(0);
        notehit[i] = checkPadHeld[i] = checkPad[i] = 0;
    }

    char _path[256];
    sprintf(_path, "assets/songs/%s/config.json", cursong.c_str());
    const JsonAsset *_config = app->assetmanager.get<JsonAsset>(getPath(_path).c_str());;

    //events 
    events_exist = false;
    sprintf(_path, "assets/songs/%s/events.json", cursong.c_str());
    if (access(getPath(_path).c_str(), F_OK) == 0) //file exists
    {
        events_exist = true;
        events = app->assetmanager.get<JsonAsset>(getPath(_path).c_str());
    }

    //next song info
    nextsong = _config->value["next"].asString();

    //load characters

    //player
    sprintf(_path, "assets/characters/%s/", _config->value["player"].asString().c_str());
    player = new Character(_path, _config->value["player"].asString() + ".json", _config->value["playerpos"][0].asFloat(), _config->value["playerpos"][1].asFloat());
    player->setFocus(_config->value["playerpos"][2].asFloat(), _config->value["playerpos"][3].asFloat(),  _config->value["playerpos"][4].asFloat());

    //opponent
    sprintf(_path, "assets/characters/%s/", _config->value["opponent"].asString().c_str());
    opponent = new Character(_path, _config->value["opponent"].asString() + ".json", _config->value["opponentpos"][0].asFloat(), _config->value["opponentpos"][1].asFloat());
    opponent->setFocus(_config->value["opponentpos"][2].asFloat(), _config->value["opponentpos"][3].asFloat(),  _config->value["opponentpos"][4].asFloat());

    //gf    
    sprintf(_path, "assets/characters/%s/", _config->value["gf"].asString().c_str());
    gf = new GFCharacter(_path, _config->value["gf"].asString() + ".json", _config->value["gfpos"][0].asFloat(), _config->value["gfpos"][1].asFloat());

    //stage
    sprintf(_path, "assets/stages/%s/%s.json", _config->value["back"].asString().c_str(), _config->value["back"].asString().c_str()); 
    curstage.load(_path, _config->value["back"].asString());

    app->assetmanager.release(_config->assetpath.c_str()); 
    //load game assets

    //load sound effects
    sfx_3 = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/intro3.wav").c_str());
    sfx_2 = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/intro2.wav").c_str());
    sfx_1 = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/intro1.wav").c_str());
    sfx_go = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/introGo.wav").c_str());

    sfx_misses[0] = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/missnote1.wav").c_str());
    sfx_misses[1] = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/missnote2.wav").c_str());
    sfx_misses[2] = app->assetmanager.get<SoundAsset>(getPath("assets/sounds/missnote3.wav").c_str());

    sprintf(_path, "assets/songs/%s/%s.bin", cursong.c_str(), cursong.c_str()); //todo implement difficulty
    app->parser.loadChart(getPath(_path).c_str());
    app->parser.songTime = -30 * app->parser.step_crochet; //always start at step -30

    sprintf(_path, "assets/songs/%s/Voices.ogg", cursong.c_str());
    if (access(getPath(_path).c_str(), F_OK) == 0) //file exists
    {
        vocals = new Audio::StreamedFile(*app->audioMixer, getPath(_path).c_str());
        sprintf(_path, "assets/songs/%s/Inst.ogg", cursong.c_str());
        inst = new Audio::StreamedFile(*app->audioMixer, getPath(_path).c_str());
    }
    else 
    {
        inst = NULL;
        sprintf(_path, "assets/songs/%s/Inst.ogg", cursong.c_str());
        vocals = new Audio::StreamedFile(*app->audioMixer, getPath(_path).c_str());
    }
    hud = app->assetmanager.get<ImageAsset>(getPath("assets/hud.png").c_str());
    icons = app->assetmanager.get<ImageAsset>(getPath("assets/icons.png").c_str());
    gamecam.camx = opponent->camx;
    gamecam.camy = opponent->camy;
    gamecam.basezoom = opponent->camzoom+CAMERA_ZOOM_OFFSET;

    gamecam.zoom = gamecam.basezoom;
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
    //Rating(std::string name, int hitwindow, float ratingmod, int score, bool splash);
    ratingData.emplace_back("sick", 45,  1,   350,  true);
    ratingData.emplace_back("good", 90,  0.7, 200, false);
    ratingData.emplace_back("bad",  135, 0.4, 100, false);
    ratingData.emplace_back("shit", 165, 0,    50, false);

    app->parser.chartdata.speed /= 6;
}

PlayStateScreen::PlayStateScreen(std::string song, bool _freeplay)
{
    initscr(song, _freeplay);
}

void PlayStateScreen::Camera::update(float ox, float oy, float oz, float px, float py, float pz) {
    if (app->parser.sections[app->parser.curStep / 16].flag & FLAG_SEC_MUSTHIT) { 
        camx.setValue(px, 0.2);
        camy.setValue(py, 0.2);
        basezoom = pz+CAMERA_ZOOM_OFFSET;
        zoom.setValue(basezoom, 0.3);
    }
    else {
        camx.setValue(ox, 0.2);
        camy.setValue(oy, 0.2);
        basezoom = oz+CAMERA_ZOOM_OFFSET;
        zoom.setValue(basezoom, 0.3);
    }
}

void PlayStateScreen::updateEvents(void) {
    for (int i = 0; i < static_cast<int>(events->value["events"].size()); i++)
    {
        const Json::Value &event = events->value["events"][i];
        if (event["eventstep"].asInt() == app->parser.curStep && app->parser.justStep)
        {
            debugLog("Playing event %s", event["eventname"].asString().c_str());
            switch (Hash::FromString(event["eventname"].asString().c_str())) {
                case "Play Animation"_h:
                    switch (Hash::FromString(event["character"].asString().c_str()))
                    {
                        case "Player"_h:
                            player->setAnim(event["animation"].asInt(), ModeNone);
                            break;
                        case "Opponent"_h:
                            opponent->setAnim(event["animation"].asInt(), ModeNone);
                            break;
                        case "GF"_h:
                            gf->setAnim(event["animation"].asInt(), ModeNone);
                            break;
                        default:
                            break;
                    }
                    break;
                case "Bump Speed Change"_h:
                    bumpfreq = event["value"].asInt();
                    break;

                default:
                    break;
            }
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
        if (events_exist)
            updateEvents();

        if (app->parser.justStep && !(app->parser.curStep % 16))
            gamecam.update(opponent->camx, opponent->camy, opponent->camzoom,
                           player->camx, player->camy, player->camzoom);

        //bump hud
        if (app->parser.justStep && !(app->parser.curStep % bumpfreq)) {
            hudcam.zoom.setValue(1.1, 1.0, 0.2); 
        }
        //bump game
        if (app->parser.justStep && !(app->parser.curStep % bumpfreq)) {
            gamecam.zoom.setValue(gamecam.basezoom+0.05, gamecam.basezoom, 0.2); 
        }
        //bump icons
        if (app->parser.justStep && !(app->parser.curStep % 4)) {
            iconcam.zoom.setValue(0.04, 0, 0.2); 
        }

        //limit health
        if (health <= 0){
            setScreen(new FreeplayScreen(0));
            return;
        }
        else if (health > 1)
            health = 1;
        
        combo.tick();
        
        updateInput();
    }
    else
    {
        app->parser.songTime += app->deltatime; 
        
        if (app->parser.justStep) {
            switch (app->parser.curStep)
            {
                case -16:
                //3
                    app->audioMixer->playBuffer(sfx_3->soundbuffer);
                    break;
                case -12:
                //2
                    countdown_alpha.setValue(255, 0, 4 * app->parser.stepsPerSecond);
                    app->audioMixer->playBuffer(sfx_2->soundbuffer);
                    break;
                case -8:
                //1
                    countdown_alpha.setValue(255, 0, 4 * app->parser.stepsPerSecond);
                    app->audioMixer->playBuffer(sfx_1->soundbuffer);
                    countdown_img.y += 91;
                    break;
                case -4:
                //go
                    countdown_alpha.setValue(255, 0, 4 * app->parser.stepsPerSecond);
                    app->audioMixer->playBuffer(sfx_go->soundbuffer);
                    countdown_img.y += 91;
                    break;
            }

        }

        //song start
        if (app->parser.curStep <= 0)
        {
            if (app->parser.songTime >= 0 && !isPlaying)
            {            
                gamecam.update(opponent->camx, opponent->camy, opponent->camzoom,
                               player->camx, player->camy, player->camzoom);
                hudcam.zoom.setValue(1.1, 1.0, 0.2); 
                gamecam.zoom.setValue(gamecam.basezoom+0.05, gamecam.basezoom, 0.2); 

                if (inst != NULL)
                    inst->play();
                vocals->play();
            }
        }
        else
        {
            if ((nextsong == "NULL" || nextsong == "null") || isfreeplay) {
                setScreen(new FreeplayScreen(0));
            }
            else {
                //reload the screen
                if ((nextsong == "NULL" || nextsong == "null"))
                    setScreen(new StoryModeScreen(0));
                else
                {
                    std::string next = nextsong;
                    setScreen(new PlayStateScreen(next, false));
                }
            }
            return;
        }
    }
    curstage.tick(gamecam.camx.getValue(), gamecam.camy.getValue());
    player->tick();
    opponent->tick();
    gf->tick();
}

void PlayStateScreen::drawHealthBar(void) {
    Gfx::RectWH<int> img = {1, 501, 320, 10};
    Gfx::RectWH<int> disp = {80, 235, img.w, 10};
    //player
    //GFX::drawTex<int>(&icons->image, &img, &disp, 0, 255, hudcam.zoom.getValue());

    //opponent
    img.y = 490;
    img.w = fabs(health - 1.0) * 320;
    disp.w = img.w;
    //GFX::drawTex<int>(&icons->image, &img, &disp, 0, 255, hudcam.zoom.getValue());
}

#define ICON_ROWS 10
#define ICON_SIZE 50
void PlayStateScreen::drawIcons(void) {
    //opponent
    int dying = (health > 0.8 ? ICON_SIZE+1 : 0);
    int i = opponent->icon;
    Gfx::RectWH<int> img = {dying+1+((i % (ICON_ROWS/2))*((1+ICON_SIZE)*2)), 
                          1+((i/(ICON_ROWS/2))*(1+ICON_SIZE)), 
                          ICON_SIZE, 
                          ICON_SIZE};

    Gfx::RectWH<int> disp = {80+static_cast<int>(abs(health - 1.0)*320)-ICON_SIZE, 210, ICON_SIZE, ICON_SIZE};
    //GFX::drawTex<int>(&icons->image, &img, &disp, 0, 255, hudcam.zoom.getValue() + iconcam.zoom.getValue());

    //player
    dying = (health < 0.2 ? ICON_SIZE+1 : 0);
    i = player->icon;
    img = {dying+1+((i % (ICON_ROWS/2))*((1+ICON_SIZE)*2)), 
           1+((i/(ICON_ROWS/2))*(1+ICON_SIZE)), 
           ICON_SIZE, 
           ICON_SIZE};

    disp = {80+static_cast<int>(abs(health - 1.0)*320), 210, ICON_SIZE, ICON_SIZE};
    //GFX::drawTex<int>(&icons->image, &img, &disp, 0, 255, hudcam.zoom.getValue() + iconcam.zoom.getValue());
}

void PlayStateScreen::draw(void)
{
    curstage.drawObjects(curstage.bgobjects, gamecam.zoom.getValue());
    gf->draw(gamecam.camx.getValue(), gamecam.camy.getValue(), gamecam.zoom.getValue());
    curstage.drawObjects(curstage.mdobjects, gamecam.zoom.getValue());

    opponent->draw(gamecam.camx.getValue(), gamecam.camy.getValue(), gamecam.zoom.getValue());
    player->draw(gamecam.camx.getValue(), gamecam.camy.getValue(), gamecam.zoom.getValue());

    curstage.drawObjects(curstage.fgobjects, gamecam.zoom.getValue());

    combo.draw(hud, hudcam.zoom.getValue());
    
    if (app->parser.curStep < 0)
        //GFX::drawTex<int>(&hud->image, &countdown_img, &countdown_disp, 0, static_cast<int>(countdown_alpha.getValue()), hudcam.zoom.getValue());

    drawDummyNotes();
    drawNotes(false);
    drawNotes(true);

    drawHealthBar();
    drawIcons();
    app->normalFont->setZoom(hudcam.zoom.getValue());
    app->normalFont->Print(Center, app->renderer->width/2+11, app->renderer->height/2+120, "Score: %d | Misses: %d   combo: %d",  score, misses, app->parser.curStep);//combo.combo);
}

PlayStateScreen::~PlayStateScreen(void)
{
    if (player != NULL)
        delete player;
    if (opponent != NULL)
        delete opponent;
    if (gf != NULL)
        delete gf;
    if (inst != NULL)
        delete inst;
    if (vocals != NULL)
        delete vocals;

    curstage.free();
    
    app->assetmanager.release(icons->assetpath.c_str()); 
    app->assetmanager.release(hud->assetpath.c_str()); 

    app->assetmanager.release(sfx_3->assetpath.c_str()); 
    app->assetmanager.release(sfx_2->assetpath.c_str()); 
    app->assetmanager.release(sfx_1->assetpath.c_str()); 
    app->assetmanager.release(sfx_go->assetpath.c_str()); 

    app->assetmanager.release(sfx_misses[0]->assetpath.c_str()); 
    app->assetmanager.release(sfx_misses[1]->assetpath.c_str()); 
    app->assetmanager.release(sfx_misses[2]->assetpath.c_str()); 

    if (events_exist)
        app->assetmanager.release(events->assetpath.c_str()); 
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

void PlayStateScreen::missedNote(int pos) {
    if (inst != NULL)
        vocals->setVolume(0,0);

    if (pos != 0) { //sustain
        int notediff = pos - app->parser.songTime;
        if (notediff < 0 && (notediff % 8) == 0) 
            health -= 0.05/2;
    }
    else { //normal note
        health -= 0.05;
        misses += 1;
        combo.combo = 0;
    }
}

void PlayStateScreen::updateInput(void)
{
    checkPad[0] = app->event.isPressed(Input::GAME_LEFT);
    checkPad[1] = app->event.isPressed(Input::GAME_DOWN);
    checkPad[2] = app->event.isPressed(Input::GAME_UP);
    checkPad[3] = app->event.isPressed(Input::GAME_RIGHT);
    checkPadHeld[0] = app->event.isHeld(Input::GAME_LEFT);
    checkPadHeld[1] = app->event.isHeld(Input::GAME_DOWN);
    checkPadHeld[2] = app->event.isHeld(Input::GAME_UP);
    checkPadHeld[3] = app->event.isHeld(Input::GAME_RIGHT);

    for (int i = 0; i < 4; i ++)
        if (!checkPadHeld[i] && notehit[i]) //reset hits
            notehit[i] = false;
  
    //handle note hits here? why not lol
    //opponent
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes[1].size()); i++)
    {
        std::vector<Note> &notes = app->parser.gamenotes[1];
    
        if (notes[i].flag & FLAG_NOTE_HIT && notes[i].sus == 0)
            continue; //dont draw if notes are hit 

        int type = notes[i].type;
        int curNotey = static_cast<int>(((notes[i].pos - app->parser.songTime) * app->parser.chartdata.speed) + notePos.opponent[type].y);

        //note is below the screen, so go back to index 0
        if (curNotey > app->renderer->height)
            break;

        //delete note if hit
        if (curNotey < notePos.opponent[type].y)
        {
            if (inst != NULL)
                vocals->setVolume(1,1);
            if (!(notes[i].flag & FLAG_NOTE_HIT) || (notes[i].sus != 0 && curNotey+(notes[i].sus * app->parser.chartdata.speed) > notePos.opponent[type].y))
                opponent->setAnim(1+type, ModeNone); //set animation
            notes[i].flag |= FLAG_NOTE_HIT;
        }

    }    
    //player
    bool justhitnote = false;
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes[0].size()); i++)
    {
        std::vector<Note> &notes = app->parser.gamenotes[0];
    
        if (notes[i].flag & FLAG_NOTE_HIT && notes[i].sus == 0)
            continue; //dont draw if notes are hit 

        int type = notes[i].type;
        int curNotey = static_cast<int>(((notes[i].pos - app->parser.songTime) * app->parser.chartdata.speed) + notePos.player[type].y);
      
        //note is below the screen, so go back to index 0
        if (curNotey > app->renderer->height)
            break;

        //delete note if offscreen
        if (curNotey < -50) {
            if (!(notes[i].flag & FLAG_NOTE_HIT))
            {
                missedNote(0);
                notes[i].flag |= FLAG_NOTE_HIT;
            }
            if (notes[i].sus == 0) {   
                deleteNote(i, 0);
                continue;
            }
        }

        float notediff = notes[i].pos - app->parser.songTime;
/*
        if (botplay && notediff <= 0)
        {
            Rating rating = judgeNote(fabs(notediff));
            if (rating.name == "sick")
                checkPad[type] = true;
        }*/

        if (checkPadHeld[type] && notes[i].sus != 0 && notediff <= static_cast<float>(ratingData[ratingData.size()-1].hitWindow)) //sustain hits
        {                    
            justhitnote = true;
            notehit[type] = true;
            if (inst != NULL)
                vocals->setVolume(1,1);
            player->setAnim(1+type, ModeNone); //play animation 

            if ((static_cast<int>(notediff) % 8) == 0) 
                health += 0.05 / 2;
           
            if (health > 1)
                health = 1;
        }
        
        if (!checkPadHeld[type] && notes[i].sus != 0 && notediff <= static_cast<float>(-ratingData[ratingData.size()-1].hitWindow)) {
            missedNote(notes[i].pos);
        }

        //check if its been hit
        if (checkPad[type] && fabs(notediff) <= static_cast<float>(ratingData[ratingData.size()-1].hitWindow) && !(notes[i].flag & FLAG_NOTE_HIT)) //shit hit window
        {                 
            justhitnote = true;   
            notehit[type] = true;
            Rating rating = judgeNote(fabs(notediff));
            if (inst != NULL)
                vocals->setVolume(1,1);
            notes[i].flag |= FLAG_NOTE_HIT;
            player->setAnim(1+type, ModeNone); //play animation 
            score += rating.score; 
            health += 0.023;
            if (health > 1)
                health = 1;
            combo.combo += 1;
            combo.spawnNew(rating.name);
            checkPad[type] = false;
        }
    }        

    if (!ghosttap && (checkPad[0] || checkPad[1] || checkPad[2] || checkPad[3]) && !justhitnote) //miss note if ghosttapping is off
    {
        missedNote(0);
        app->audioMixer->playBuffer(sfx_misses[rand() % (COUNT_OF(sfx_misses))]->soundbuffer);

        //miss animation
        if (checkPad[0] || checkPad[1] || checkPad[2] || checkPad[3])
        {
            int anim = 0;
            for (int i = 0; i < 4; i++)
                if (checkPadHeld[i]) {
                    anim = i;
                    break;
                }
            player->setAnim(1+anim, ModeNone);
        }
    }
}
