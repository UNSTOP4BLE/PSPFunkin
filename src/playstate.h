#pragma once

#include "screen.h"
#include "character.h"
#include "stage.h"
#include "psp/tween.h"
#include "psp/audio_streamed_file.h"

struct Pos 
{
    int x, y;
};

struct NotePos 
{
    Pos player[4];
    Pos opponent[4];
};

class Rating
{
public:
    Rating(std::string name, int score, bool splash, float ratingmod, int hitwindow);
    std::string name;
    GFX::RECT<int> img;
    int hitWindow; //ms
    float ratingMod;
    int score;
    bool noteSplash;
};

class PlayStateScreen : public Screen {
public:
    PlayStateScreen(void);
    void update(void);
    void draw(void);
    ~PlayStateScreen(void);
private:    
    class Camera {
    public:
        void update(float ox, float oy, float oz, float px, float py, float pz);
        Tween<float, QuadInEasing, ChronoTimeSource> camx, camy;
        Tween<float, QuadInEasing, ChronoTimeSource> zoom;
    } gamecam, hudcam;
    void initscr(void);
    void freescr(void);
    void drawDummyNotes(void);
    void drawSustain(int note, float y, int type);
    void drawNotes(void);
    void increaseScore(int note);
    Rating judgeNote(float diff);
    void missedNote();
    void updateInput(void);
    Character *player;
    Character *opponent;
    Character *gf;
    Stage curstage;
    GFX::Texture *hud;
    Audio::StreamedFile *inst; 
    Audio::StreamedFile *vocals; 
    std::vector<Rating> ratingData;
    std::string cursong;
    std::string nextsong;
    bool checkPad[4];
    bool checkPadHeld[4];
    NotePos notePos;
    int curStep;
    int score;
};