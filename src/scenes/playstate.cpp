//to rewrite
#include "playstate.hpp"
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
            curnote.pos = j_curnote[0].asFloat();
            curnote.type = j_curnote[1].asInt();
            curnote.sustain = j_curnote[2].asFloat();
      
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
    const ASSETS::JsonAsset *posjson = g_app.assets.get<ASSETS::JsonAsset>(FS::getFilePath("assets/notepositions.json"));
    const Json::Value &positions = posjson->value["notepositions"];
    for (int i = 0; i < NUM_NOTES; i++) {
        chart.opponentnotes.positions[i].x = positions["opponent"][i][0].asInt();
        chart.opponentnotes.positions[i].y = positions["opponent"][i][1].asInt();
        chart.playernotes.positions[i].x = positions["player"][i][0].asInt();
        chart.playernotes.positions[i].y = positions["player"][i][1].asInt();
    }
    g_app.assets.release(posjson->assetpath);

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
    for (size_t i = container.cullingindex; i < container.notes.size(); i++) {
        auto &note = container.notes[i];
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