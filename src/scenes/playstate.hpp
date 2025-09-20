//to rewrite
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

class NoteContainer {
public:
    inline void init(void) {
        cullingindex = 0;
        std::sort(notes.begin(), notes.end(), compareByPos); 
    }
    inline int getSize(void) {return notes.size();}
    std::vector<NoteData> notes;
    int cullingindex;
private:
    static inline bool compareByPos(const NoteData &a, const NoteData &b) {
        return a.pos < b.pos;
    }
};

struct ChartData {
    NoteContainer playernotes;
    NoteContainer opponentnotes;
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
    void drawNotes(NoteContainer &container, GFX::XY<int32_t> pos);
    AUDIO::StreamedFile *inst, *voices; 
    ChartData chart;
    float songtime;
    int songstep;
};