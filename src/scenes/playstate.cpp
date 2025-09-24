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
            curnote.type = j_curnote[1].asInt();
            curnote.sus = j_curnote[2].asFloat();
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

    chart.scrollspeed = j_song["speed"].asFloat() * PIXELS_PER_MS;
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

    static const Rating miss{"null", -1, -1};
    return miss;
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

        //player input

        //set up controls
        INPUT::Button gameInputs[NUM_NOTES] = {
            INPUT::GAME_LEFT,
            INPUT::GAME_DOWN,
            INPUT::GAME_UP,
            INPUT::GAME_RIGHT
        };

        bool inputs[NUM_NOTES];
        bool inputsHeld[NUM_NOTES];
        //update inputs
        for (int i = 0; i < NUM_NOTES; i++) {
            inputs[i] = g_app.input.isPressed(gameInputs[i]);
            inputsHeld[i] = g_app.input.isHeld(gameInputs[i]);
        }

        auto &playercontainer = chart.playernotes;
        for (int i = playercontainer.cullingindex; i < playercontainer.notes.size(); i++) {
            auto &note = playercontainer.notes[i];
            bool issus = (note.flag & FLAG_SUSTAIN);
            float position = note.pos-songtime; //milliseconds

            //sustain hits happen no matter if the note is hit or missed
            if (issus) {
                if (songtime >= note.pos && songtime <= note.pos+note.sus) {
                    if (inputsHeld[note.type]) {//hitting sustain
                        printf("hi this is a sustain\n");
                    }
                    else //missing sustain
                        printf("hi u suck\n");
                }
            }

            if (note.flag & (FLAG_HIT | FLAG_MISSED)) //no point in processing note if its hit or missed
                continue;
            
            const auto& worstwindow = ratings.back().hitwindow; //worst rating possible, has largest hitwindow, in this case "shit"
  
            //cull notes that cant be hit yet
            if (position > worstwindow) {
//                printf("culling note %d\n", i);
                break;
            }
            //note flew by, missed a note
            if (position < -worstwindow){
                printf("missed note %d\n", i);
                note.flag |= FLAG_MISSED;
                continue;
            }

            //pseudo botplay
//            if (position <= 0)
  //              inputs[note.type] = true;

            //hit a note
            if (inputs[note.type]) {
                const auto &hitrating = judgeNote(fabs(position));
                
                note.flag |= FLAG_HIT;
                //position is how early or late you hit
                printf("hit a note %f\n", position);
                continue;
            }
        }

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
    for (int i = container.cullingindex; i < container.notes.size(); i++) {
        auto &note = container.notes[i];
        bool issus = (note.flag & FLAG_SUSTAIN);

        const GFX::XY<int32_t> &offset = container.positions[note.type];
        float y = NotePosPX(note);
        GFX::RECT<int32_t> notepos = {offset.x, offset.y+static_cast<int>(y), notesizePX, notesizePX};
        
        //always render sustain
        if (issus) {
            float sush = SustainPX(note);
            GFX::RECT<int32_t> suspos = {notepos.x+10, notepos.y+notepos.h, 20, static_cast<int>(sush)};
            g_app.renderer->drawRect(suspos, 0, 0xFF0000FF);
        }

        if (note.flag & FLAG_HIT) //no point in processing note if its hit
            continue;


        //cull notes that went offscreen
//        if (y < 0) { //incomplete, has to use sustain too
  //          container.cullingindex = i+1;
    //        continue;
      //  }
        //cull notes that cant be seen yet
        if (y > GFX::SCREEN_HEIGHT) 
            break;

        //note
        if (note.flag & FLAG_MISSED)
            g_app.renderer->drawRect(notepos, 0, 0x808080FF); // for testing draw grey note for a missed note
        else
            g_app.renderer->drawRect(notepos, 0, 0xFFFFFFFF);
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

    //todo rewrite
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