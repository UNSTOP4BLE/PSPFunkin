//to rewrite
#include "playstate.hpp"
#include <cstdio>
#include <json/json.h>
#include <fstream>
#include <cassert>

#include "../engine/file.hpp"

PlayStateSCN::PlayStateSCN(void) {
    std::string songname = "assets/songs/bopeebo/";
    //todo diffictuly
    const ASSETS::JsonAsset *songchart = g_app.assets.get<ASSETS::JsonAsset>(FS::getFilePath(songname + "bopeebo.json"));
    const Json::Value &j_chart = songchart->value;

    //parse file
    auto j_song = j_chart["song"];
    auto j_notes = j_song["notes"];

    //sections
    for (int cs = 0; cs < j_notes.size(); cs++) {
        auto &j_cursec = j_notes[cs];
        SectionData cursec;
        cursec.musthit = j_cursec["mustHitSection"].asBool();

        chart.sections.push_back(cursec);
        //notes
        for (int cn = 0; cn < j_cursec["sectionNotes"].size(); cn++) {
            auto &j_curnote = j_cursec["sectionNotes"][cn];
            NoteData curnote;
            curnote.flag = FLAG_NONE;

            curnote.pos = j_curnote[0].asFloat();
            curnote.sus_start = curnote.pos;
            curnote.type = j_curnote[1].asInt();
            curnote.sus = j_curnote[2].asFloat();
            curnote.initialsus = curnote.sus;
            if (curnote.sus > 0)
                curnote.flag |= FLAG_SUSTAIN;
            //determine target container, decides if note is player or opponent
            auto &target = (cursec.musthit ^ (curnote.type >= NUM_NOTES)) ? chart.playernotes.notes : chart.opponentnotes.notes;
            //normalize
            if (curnote.type >= NUM_NOTES) 
                curnote.type -= NUM_NOTES;
            assert(curnote.type < NUM_NOTES); //wont work for special notes im sorry lol

            target.push_back(curnote);
        }
    }
    //sort notes
    chart.opponentnotes.init();
    chart.playernotes.init();

    chart.scrollspeed = j_song["speed"].asFloat();
    chart.bpm = j_song["bpm"].asInt();
    chart.hasvoices = j_song["needsVoices"].asBool();
    chart.crochet = (60 / chart.bpm) * 1000;
    chart.stepcrochet = chart.crochet / 4;
    step_per_sec = (60 / TICKS_PER_BEAT) / chart.bpm;

    printf("song has %d notes and %zu sections\n", chart.playernotes.getSize()+chart.opponentnotes.getSize(), chart.sections.size());
    g_app.assets.release(songchart->assetpath);

    //init
    songtime = -30 * chart.stepcrochet; //start at step -30

    inst = new AUDIO::StreamedFile(g_app.audiomixer, FS::getFilePath(songname + "Inst.ogg").c_str());
    
    if (chart.hasvoices)
        voices = new AUDIO::StreamedFile(g_app.audiomixer, FS::getFilePath(songname + "Voices.ogg").c_str());

    //read note positions 
    const ASSETS::JsonAsset *posjson = g_app.assets.get<ASSETS::JsonAsset>(FS::getFilePath("assets/notes.json"));
    const Json::Value &positions = posjson->value["notepositions"];
    notesizePX = posjson->value["note_size"].asInt();
    int spacing = posjson->value["note_spacing"].asInt();
    int notestep = notesizePX + spacing;

    for (int i = 0; i < NUM_NOTES; i++) {
        //opponent notes
        chart.opponentnotes.positions[i].x = positions["opponent"][i][0].asInt() + notestep * i;
        chart.opponentnotes.positions[i].y = positions["opponent"][i][1].asInt();

        //player notes (flipped horizontally)
        int rev = NUM_NOTES - 1 - i;
        chart.playernotes.positions[i].x = GFX::SCREEN_WIDTH - (positions["player"][i][0].asInt() + notestep * rev) - notesizePX;
        chart.playernotes.positions[i].y = positions["player"][i][1].asInt();
    }

    //read the rating data
    const Json::Value &ratingdata = posjson->value["ratings"];
    for (int i = 0; i < ratingdata.size(); i++) {
        auto &currating = ratingdata[i];
        ratings.emplace_back(currating["rating"].asString(), currating["hitwindow"].asInt(), currating["score"].asInt());
    }
    
    g_app.assets.release(posjson->assetpath);

    health = 0.5; //middle of health bar
}

const Rating& PlayStateSCN::judgeNote(float diff) const {
    for (const auto& rating : ratings) {
        if (diff <= rating.hitwindow) {
            return rating;
        }
    }

    static const Rating fake{"null", -1, -1};
    return fake;
}

void PlayStateSCN::update(void) {
    bool playing = inst->isPlaying() || (chart.hasvoices && voices->isPlaying());
    
    if (playing) { //song is playing  
        auto track = inst; //sync to vocals or instrumental
        inst->process();
        if (chart.hasvoices) {
            voices->process();
            track = voices;
        }
        songtime = track->getChannel().getTime()*1000;

        //set up controls
        bool inputs[NUM_NOTES] = {g_app.input.isPressed(INPUT::GAME_LEFT),
                                 g_app.input.isPressed(INPUT::GAME_DOWN), 
                                 g_app.input.isPressed(INPUT::GAME_UP), 
                                 g_app.input.isPressed(INPUT::GAME_RIGHT)};
        bool inputsheld[NUM_NOTES] = {g_app.input.isHeld(INPUT::GAME_LEFT),
                                     g_app.input.isHeld(INPUT::GAME_DOWN), 
                                     g_app.input.isHeld(INPUT::GAME_UP), 
                                     g_app.input.isHeld(INPUT::GAME_RIGHT)};
    
        //player input
        auto &playercontainer = chart.playernotes;

        int processctrdebug = 0;
        for (int i = playercontainer.cullingindex; i < playercontainer.notes.size(); i++) {
            auto &note = playercontainer.notes[i];

            if (note.getNoteY(chart.scrollspeed, songtime) > GFX::SCREEN_HEIGHT) //offscreen
                break; //can break here cuz the notes are sorted

            float diff = note.pos - songtime; 
            processctrdebug ++;
            //hold note hits
            bool issus = (note.flag & FLAG_SUSTAIN);
            float fullsustain = note.sus_start+note.sus;
            if (songtime >= note.sus_start && songtime <= fullsustain) {
                float remainingsustain = fullsustain - songtime; //remaining amount of sustain in ms
    
                if (inputsheld[note.type]) {
                    note.sus_start = songtime;
                    note.sus = remainingsustain;
                    printf("hit sustain here!remaining%f\n", remainingsustain);
                }
                else 
                    printf("missed sustain here!remaining%f\n", remainingsustain);
            }   

            if (note.flag & (FLAG_HIT | FLAG_MISSED) || diff > ratings.back().hitwindow) //no point in processing note if its hit or cant be hit
                continue;
                
            if (diff < -ratings.back().hitwindow) { // u suck ass and missed a note, went offscreen
                note.flag |= FLAG_MISSED;
                printf("you missed\n");
                health -= HEALTH_INC_AMOUNT;
                continue;
            }
                
            //check if any button pressed
            bool anyinput = false;
            for (int j = 0; j < NUM_NOTES; j++) {
                if (inputs[j]) {
                    anyinput = true;
                    break;
                }
            }
            //normal note hits
            if (anyinput) {
                diff = fabs(diff); 
                //hit a note
                if (inputs[note.type]) {
                    auto rating = judgeNote(diff);

                    note.flag |= FLAG_HIT;
                    printf("diff %f\n", diff);
                    printf("hit note%s\n", rating.name.c_str());
                    inputs[note.type] = false;

                    health += HEALTH_INC_AMOUNT;
                    continue;
                }
            }
        }
//        printf("processed %d notes this frame\n", processctrdebug);
        
        //opponent input (basically botplay)
        auto &oppcontainer = chart.opponentnotes;
        for (int i = oppcontainer.cullingindex; i < oppcontainer.notes.size(); i++) {
            auto &note = oppcontainer.notes[i];
            if (note.flag & FLAG_HIT) //no point in processing note if its hit
                continue;
            
            float diff = note.pos - songtime; //no absolute value here 
            if (diff > 0) //dont process note if opponent cant hit it
                break; //can break here because i sorted the notes

            if (diff <= 0)
                note.flag |= FLAG_HIT;
        }

    }
    else { //song didnt start yet
        songtime += g_app.deltatime;
        //song start
        if (songstep <= 0)
        {
            if (songtime >= 0 && !playing) {            
//                gamecam.update(opponent->camx, opponent->camy, opponent->camzoom,
  //                             player->camx, player->camy, player->camzoom);
    //            hudcam.zoom.setValue(1.1, 1.0, 0.2); 
      //          gamecam.zoom.setValue(gamecam.basezoom+0.05, gamecam.basezoom, 0.2); 
                
                inst->play();
                if (chart.hasvoices)
                    voices->play();
            }
        }
        //song end
        else
        {
            //go to menu or load next song
            return;
        }
    }

}

void PlayStateSCN::drawNotes(NoteContainer &container) {
    int tempsusend = 24; //pixels 
    for (int i = container.cullingindex; i < container.notes.size(); i++) {
        auto &note = container.notes[i];
        bool issus = (note.flag & FLAG_SUSTAIN);
        if (note.flag & FLAG_HIT && !issus) //no point in processing note if its hit
            continue;
        const GFX::XY<int32_t> &pos = container.positions[note.type];

        //cull notes off screen
        int y = note.getNoteY(chart.scrollspeed, songtime);
        int sustainh = issus ? note.getSustainH(chart.scrollspeed) : 0;

        if (y < -notesizePX-pos.y-note.getInitialSustainH(chart.scrollspeed)) {
            container.cullingindex = i+1; //+1, render the next note
            continue;
        }
        if (y > GFX::SCREEN_HEIGHT) {
            break;
        }

        //note
        GFX::RECT<int32_t> notepos = {pos.x, pos.y+y, notesizePX, notesizePX};
        if (!(note.flag & FLAG_HIT)) //only gets run on sustain notes so if you hit a note it doesnt mean you hit the whole sustain
            g_app.renderer->drawRect(notepos, 0, 0xFFFFFFFF);

        if (issus) {
            //sustain body
            GFX::RECT<int32_t> sustainpos = {pos.x+10, pos.y+note.getSusY(chart.scrollspeed, songtime)+notepos.h, 20, sustainh};
            g_app.renderer->drawRect(sustainpos, 0, 0xFF0000FF);
//            //sustain end
  //          GFX::RECT<int32_t> sustainend = {sustainpos.x, sustainpos.y+sustainpos.h, sustainpos.w, tempsusend};
    //        g_app.renderer->drawRect(sustainend, 0, 0xFFFF00FF);
        }
    }
}

void PlayStateSCN::drawDummyNotes(NoteContainer &container) {
    for (int i = 0; i < NUM_NOTES; i++) {
        const GFX::XY<int32_t> &pos = container.positions[i];
        
        GFX::RECT<int32_t> notepos = {pos.x, pos.y, 40, 40};
        g_app.renderer->drawRect(notepos, 0, 0xFFF00FFF);
    }
}

void PlayStateSCN::draw(void) {
    //opponent
    drawDummyNotes(chart.opponentnotes);
    drawNotes(chart.opponentnotes);
    //player
    drawDummyNotes(chart.playernotes);
    drawNotes(chart.playernotes);

    //draw health bar 
    int hb_width = GFX::SCREEN_WIDTH - 226;
    int hb_height = 8; 
    int spacing = 1; //black border for health bar

    int hb_x = (GFX::SCREEN_WIDTH - hb_width) / 2;
    int hb_y = GFX::SCREEN_HEIGHT - 20 - hb_height;         

    GFX::RECT<int32_t> hb = {hb_x, hb_y, hb_width, hb_height};
    g_app.renderer->drawRect(hb, 0, 0x000000FF); //black base

    GFX::RECT<int32_t> hb_opp = {hb.x+spacing, hb.y+spacing, hb.w-spacing*2, hb.h-spacing*2};
    g_app.renderer->drawRect(hb_opp, 0, 0xFF0000FF); //red
}

PlayStateSCN::~PlayStateSCN(void) {
    delete inst;
    if (chart.hasvoices)
        delete voices;
}  