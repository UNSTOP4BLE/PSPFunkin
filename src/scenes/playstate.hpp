//to rewrite
#pragma once

#include "../app.hpp"
#include "../engine/audio_streamed_file.hpp"

constexpr int TICKS_PER_BEAT = 4;
constexpr int NUM_NOTES = 4;
//cursed unit fnf uses, no idea why they did it like this, this depends on the fucking resolution
constexpr float PIXELS_PER_MS = 0.45 * GFX::SCREEN_HEIGHT / 720;

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
    GFX::XY<int32_t> positions[NUM_NOTES]; //on screen positions
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
};


class PlayStateSCN : public SCENE::Scene {
public:
    PlayStateSCN(void);
    void update(void);
    void draw(void);
    ~PlayStateSCN(void); 
private:
    void drawNotes(NoteContainer &container);
    void drawDummyNotes(NoteContainer &container);
    AUDIO::StreamedFile *inst, *voices; 
    ChartData chart;
    float songtime;
    int songstep;
    float step_per_sec;
};