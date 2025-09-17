#include "playstate.hpp"
#include <json/json.h>
#include <fstream>
#include <cassert>

inline bool compareByPos(const NoteData &a, const NoteData &b) {
    return a.pos < b.pos;
}

PlayStateSCN::PlayStateSCN(void) {
    std::string songname = "assets/songs/bopeebo/";
    //todo diffictuly
    std::ifstream file(getFilePath(songname + "bopeebo.json"));
    Json::Reader reader;
    Json::Value j_chart;
    assert(reader.parse(file, j_chart));   

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
            auto &target = (cursec.musthit ^ (curnote.type >= 4)) ? chart.opponentnotes : chart.playernotes;
            if (curnote.type >= 4) 
                curnote.type -= 4;
       
            target.push_back(curnote);

        }
    }
    //sort notes
    std::sort(chart.opponentnotes.begin(), chart.opponentnotes.end(), compareByPos); 
    std::sort(chart.playernotes.begin(), chart.playernotes.end(), compareByPos); 

    chart.scrollspeed = j_song["speed"].asFloat()/10;
    chart.bpm = j_song["bpm"].asInt();
    chart.hasvoices = j_song["needsVoices"].asBool();
    chart.crochet = (60 / chart.bpm) * 1000;
    chart.stepcrochet = chart.crochet / 4;
    chart.step_per_sec = 15/chart.bpm;

    printf("song has %zu notes and %zu sections\n", chart.playernotes.size()+chart.opponentnotes.size(), chart.sections.size());

    file.close();

    //init
    songtime = -30 * chart.stepcrochet; //start at step -30

    inst = new Audio::StreamedFile(g_app.audiomixer, getFilePath(songname + "Inst.ogg").c_str());
    
    if (chart.hasvoices)
        voices = new Audio::StreamedFile(g_app.audiomixer, getFilePath(songname + "Voices.ogg").c_str());
}

void PlayStateSCN::update(void) {
    bool playing = inst->isPlaying() || (chart.hasvoices && voices->isPlaying());
    songstep = songtime / chart.stepcrochet;
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

void PlayStateSCN::draw(void) {
    for (auto &note : chart.playernotes) {
        g_app.renderer->drawRect({32*note.type, static_cast<int>((note.pos - songtime)*chart.scrollspeed), 32, 32}, 0, 0xFFFFFF);
    }
    for (auto &note : chart.opponentnotes) {
        g_app.renderer->drawRect({GFX::SCREEN_WIDTH/2 + 32*note.type, static_cast<int>((note.pos - songtime)*chart.scrollspeed), 32, 32}, 0, 0xFFFFFF);
    }
}

PlayStateSCN::~PlayStateSCN(void) {
    delete inst;
    if (chart.hasvoices)
        delete voices;
}  