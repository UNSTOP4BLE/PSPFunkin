//to rewrite
#include "playstate.hpp"
#include <cstdio>
#include <json/json.h>
#include <fstream>
#include <cassert>

#include "../engine/file.hpp"

PlayStateSCN::PlayStateSCN(void) {
    std::string songname = "assets/songs/no-more-woah/";
    //todo diffictuly
    const ASSETS::JsonAsset *songchart = g_app.assets.get<ASSETS::JsonAsset>(FS::getFilePath(songname + "no-more-woah.json"));
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
            curnote.pos = j_curnote[0].asFloat();
            curnote.type = j_curnote[1].asInt();
            curnote.sustain = j_curnote[2].asFloat();
            curnote.flag = FLAG_NONE;
      
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
    for (int i = 0; i < NUM_NOTES; i++) {
        chart.opponentnotes.positions[i].x = positions["opponent"][i][0].asInt();
        chart.opponentnotes.positions[i].y = positions["opponent"][i][1].asInt();
        chart.playernotes.positions[i].x = positions["player"][i][0].asInt();
        chart.playernotes.positions[i].y = positions["player"][i][1].asInt();
    }
    //read the rating data
    const Json::Value &ratingdata = posjson->value["ratings"];
    for (int i = 0; i < ratingdata.size(); i++) {
        auto &currating = ratingdata[i];
        ratings.emplace_back(currating["rating"].asString(), currating["hitwindow"].asInt(), currating["score"].asInt());
    }
    
    g_app.assets.release(posjson->assetpath);

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

        bool inputs[NUM_NOTES] = {g_app.input.isPressed(INPUT::GAME_LEFT),
                                 g_app.input.isPressed(INPUT::GAME_DOWN), 
                                 g_app.input.isPressed(INPUT::GAME_UP), 
                                 g_app.input.isPressed(INPUT::GAME_RIGHT)};
    
        //player input
        auto &playercontainer = chart.playernotes;

        uint32_t ignoreflags = (FLAG_HIT | FLAG_MISSED);
        for (int i = playercontainer.cullingindex; i < playercontainer.notes.size(); i++) {
            auto &note = playercontainer.notes[i];
            if (note.flag & ignoreflags) //no point in processing note if its hit
                continue;

            float diff = note.pos - songtime;
            if (diff < -ratings.back().hitwindow) { // u suck ass and missed a note
                note.flag |= FLAG_MISSED;
                printf("you missed\n");
            }

            //check if any button pressed
            bool anyinput = false;
            for (int j = 0; j < NUM_NOTES; j++) {
                if (inputs[j]) {
                    anyinput = true;
                    break;
                }
            }

            if (anyinput) {
                diff = fabs(diff); 
                //hit a note
                if (diff <= ratings.back().hitwindow && inputs[note.type]) {
                    auto rating = judgeNote(diff);

                    note.flag |= FLAG_HIT;
                    printf("diff %f\n", diff);
                    printf("hit note%s\n", rating.name.c_str());
                    inputs[note.type] = false;
                    continue;
                }
                //random key pressed, handle ghosttaping here
                printf("pressed a random key\n");
                break;
            }
        }
        
        //opponent input (basically botplay)
        auto &oppcontainer = chart.opponentnotes;
        for (int i = oppcontainer.cullingindex; i < oppcontainer.notes.size(); i++) {
            auto &note = oppcontainer.notes[i];
            if (note.flag & FLAG_HIT) //no point in processing note if its hit
                continue;
            
            float diff = note.pos - songtime; //no absolute value here 
            if (diff > 0) //dont process note if you cant hit it
                break; //can break here because i sorted the notes

            if (diff <= 0)
                note.flag |= FLAG_HIT;
        }

    }
    else { //song didnt start yet
        songtime += g_app.deltatime;
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
        if (note.flag & FLAG_HIT) //no point in processing note if its hit
            continue;
        const GFX::XY<int32_t> &pos = container.positions[note.type];
        
        //note position and song time in ms
        int y = static_cast<int>(PIXELS_PER_MS * (note.pos - songtime) * chart.scrollspeed);

        //cull notes off screen, this does assume a note is never larger than 40x40 so yeah gotta change this
        if (y < -pos.y-40) {
            container.cullingindex = i;
            continue;
        }
        if (y > GFX::SCREEN_HEIGHT) {
            break;
        }

        GFX::RECT<int32_t> notepos = {pos.x, pos.y+y, 40, 40};
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
}

PlayStateSCN::~PlayStateSCN(void) {
    delete inst;
    if (chart.hasvoices)
        delete voices;
}  