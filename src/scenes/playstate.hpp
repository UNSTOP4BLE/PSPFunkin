#pragma once

#include "../app.hpp"
#include "../engine/audio_streamed_file.hpp"

struct NoteData {
    float pos;
    int type;
    float sustain;    
};

struct SectionData {
    bool musthit;
};

struct ChartData {
    std::vector<NoteData> playernotes;
    std::vector<NoteData> opponentnotes;
    std::vector<SectionData> sections;
    float scrollspeed;
    float bpm;
    bool hasvoices;
    float stepcrochet;
    float crochet;
    float step_per_sec;
};

class PlayStateSCN : public SCENE::Scene {
public:
    PlayStateSCN(void);
    void update(void);
    void draw(void);
    ~PlayStateSCN(void); 
private:
    Audio::StreamedFile *inst, *voices; 
    ChartData chart;
    float songtime;
    int songstep;
};