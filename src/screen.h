#pragma once

#include "psp/gfx.h"
#include "psp/audio_streamed_file.h"
#include "psp/tween.h"

#include "character.h"
#include "chartparser.h"
#include "stage.h"

class Screen {
public:
    inline Screen(void) {}
    virtual void update(void) {}
    virtual void draw(void) {}
    virtual ~Screen(void) {}
};

class TitleScreen : public Screen {
public:
    TitleScreen(void);
    void update(void);
    void draw(void);
    ~TitleScreen(void);
private:
    GFX::Texture *ng;
    Anim_OBJECT titleGF;
    std::string funnymessage[2];
    Audio::StreamedFile *freaky; 
    Audio::AudioBuffer *confirm;
    int curStep;
};

class MainMenuScreen : public Screen {
public:
    MainMenuScreen(void);
    void update(void);
    void draw(void);
    ~MainMenuScreen(void); 
private:
    GFX::Texture *background;
    int selection;
    Tween<float, QuadInEasing> backgroundy;
};

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
    std::string name;
    GFX::RECT<int> img;
    int hitWindow; //ms
    float ratingMod;
    int score;
    bool noteSplash = true;
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
        float camx, camy;
        float zoom;
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

void ErrMSG(const char *format, ...);
void setScreen(Screen *scr);
void setScreenCol(int color);