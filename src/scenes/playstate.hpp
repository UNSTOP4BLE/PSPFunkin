//to rewrite
#pragma once

#include "../app.hpp"
#include "../engine/audio_streamed_file.hpp"

constexpr int TICKS_PER_BEAT = 4;
constexpr int NUM_NOTES = 4;
//cursed unit fnf uses, no idea why they did it like this, this depends on the fucking resolution, multiply scroll speed by this factor to make the notes render properly
constexpr double PIXELS_PER_MS = 0.45 * GFX::SCREEN_HEIGHT / 720;
constexpr float MAX_HEALTH = 2;

// The amount of health the player gains when hitting a note with the SICK rating.
constexpr float HEALTH_SICK_BONUS = 1.5 / 100.0 * MAX_HEALTH; // +1.0%

// The amount of health the player gains when hitting a note with the GOOD rating
constexpr float HEALTH_GOOD_BONUS = 0.75 / 100.0 * MAX_HEALTH; // +0.75%

// The amount of health the player gains when hitting a note with the BAD rating
constexpr float HEALTH_BAD_BONUS = 0.0 / 100.0 * MAX_HEALTH; // +0.0%

// The amount of health the player gains (or loses) when hitting a note with the SHIT rating
constexpr float HEALTH_SHIT_BONUS = -1.0 / 100.0 * MAX_HEALTH; // -1.0%

// The amount of health the player gains, while holding a hold note, per second
constexpr float HEALTH_HOLD_BONUS = 6.0 / 100.0 * MAX_HEALTH; // +6.0% / second

// The amount of health the player loses upon letting go of a hold note, per second remaining
constexpr float HEALTH_HOLD_DROP_PENALTY = -4.5 / 100.0 * MAX_HEALTH; // -4.5% / second

// The amount of health the player loses upon missing a note
constexpr float HEALTH_MISS_PENALTY = -2.0 / 100.0 * MAX_HEALTH; // -2.0%

// The amount of health the player loses upon pressing a key when no note is there
constexpr float HEALTH_GHOST_PENALTY = -2.0 / 100.0 * MAX_HEALTH; // -2.0%

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
    double suspos;      
    double initialsus;      
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
    GFX::XY<int32_t> positions[NUM_NOTES]; //on screen offsets, loaded from json file
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
    double scrollspeed;
    double bpm;
    bool hasvoices;
    double stepcrochet;
    double crochet;
};


class PlayStateSCN : public SCENE::Scene {
public:
    PlayStateSCN(void);
    void update(void);
    void draw(void);
    ~PlayStateSCN(void); 
private:
    double SustainPX(double sus) {return std::trunc(sus * chart.scrollspeed);};
    double NotePosPX(double pos) {return std::trunc((pos - songtime) * chart.scrollspeed);};

    const Rating& judgeNote(double diff) const;

    void hitSustain(void);
    void missSustain(void);
    void hitNote(NoteData &note, double diff);
    void missNote(NoteData &note);
    void ghostPenalty(void);

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