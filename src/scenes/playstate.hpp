//to rewrite
#pragma once

#include "../app.hpp"
#include "../engine/audio_streamed_file.hpp"

constexpr int TICKS_PER_BEAT = 4;
constexpr int NUM_NOTES = 4;
//cursed unit fnf uses, no idea why they did it like this, this depends on the fucking resolution
constexpr float PIXELS_PER_MS = 0.45 * GFX::SCREEN_HEIGHT / 720;
constexpr float HEALTH_INC_AMOUNT = 0.05;

//note stuff
enum NoteFlags {
    FLAG_NONE    = 0,
    FLAG_HIT     = 1 << 0,
    FLAG_MISSED  = 1 << 1,
    FLAG_SUSTAIN = 1 << 2
};

struct NoteData {
    double pos;
    int type;
    double sus;      
    uint32_t flag;
};

struct SectionData {
    bool musthit;
};

// sick, bad, shit etc
struct Rating {
    Rating(const std::string& n, int hw, int s) : name(n), hitwindow(hw), score(s) {}
    
    std::string name;
    int hitwindow;
    int score;
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
    double SustainPX(const NoteData &note) {return note.sus * chart.scrollspeed;};
    double NotePosPX(const NoteData &note) {return (note.pos - songtime) * chart.scrollspeed;};

    const Rating& judgeNote(double diff) const;
    void drawNotes(NoteContainer &container);
    void drawDummyNotes(NoteContainer &container);
    AUDIO::StreamedFile *inst, *voices; 
    ChartData chart;
    std::vector<Rating> ratings;
    double songtime;
    int songstep;
    float step_per_sec;
    int notesizePX;
    float health;
};