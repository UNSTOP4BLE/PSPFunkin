#pragma once

#include "screen.h"
#include "character.h"
#include "stage.h"
#include "psp/tween.h"
#include "psp/audio_streamed_file.h"
#include "combo.h"

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
    Rating(std::string name, int hitwindow, float ratingmod, int score, bool splash);
    std::string name;
    int hitWindow; //ms
    float ratingMod;
    int score;
    bool noteSplash;
    GFX::RECT<int> img;
};

#define SUSTAIN_CLIPHEIGHT 16

class PlayStateScreen : public Screen {
public:
    PlayStateScreen(std::string song, bool _freeplay);
    void update(void);
    void draw(void);
    ~PlayStateScreen(void);
private:    
    class Camera {
    public:
        void update(float ox, float oy, float oz, float px, float py, float pz);
        Tween<float, QuadInOutEasing, Chrono> camx, camy;
        Tween<float, QuadInOutEasing, Chrono> zoom;
        float basezoom;
    } gamecam, hudcam, iconcam;
    void initscr(std::string song, bool _freeplay);
    void drawHealthBar(void);
    void drawIcons(void);
    void deleteNote(int &note, bool opponent);
    void drawDummyNotes(void);
    void drawSustain(int note, float y, int type, bool isopponent);
    void drawNotes(bool isopponent);
    Rating judgeNote(float diff);
    void missedNote(float y, int sustainlength);
    void updateInput(void);
    //characters
    Character *player;
    Character *opponent;
    GFCharacter *gf;
    //stage
    Stage curstage;
    //textures
    GFX::Texture *hud;
    GFX::Texture *icons;
    //audio
    Audio::StreamedFile *inst; 
    Audio::StreamedFile *vocals; 
    //other variables
    std::vector<Rating> ratingData;
    std::string cursong;
    bool isfreeplay;
    std::string nextsong;
    NotePos notePos;
    //gameplay
    Audio::AudioBuffer *sfx_3;
    Audio::AudioBuffer *sfx_2;
    Audio::AudioBuffer *sfx_1;
    Audio::AudioBuffer *sfx_go;
    Audio::AudioBuffer *sfx_misses[3];
    GFX::RECT<int> countdown_img;
    GFX::RECT<int> countdown_disp;
    Tween<float, QuadInOutEasing, Chrono> countdown_alpha;
    Combo combo;
    bool checkPad[4];
    bool checkPadHeld[4];
    int score;
    double health;
    int misses;
    bool ghosttap;
    bool botplay;
    //note animation
    Tween<float, QuadInOutEasing, Chrono> noteframe[4];
    bool notehit[4];
};