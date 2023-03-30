#pragma once

#include "main.h"
#include "psp/glib2d.h"
#include "psp/audio_streamed_file.h"

#include "character.h"
#include "chartparser.h"

class Screen {
public:
    inline Screen(void) {}
    virtual void load(void) {}
    virtual void update(void) {}
    virtual void draw(void) {}
    virtual void deload(void) {}
    virtual ~Screen(void) {}
};

class TitleScreen : public Screen {
public:
    void load(void); 
    void update(void);
    void draw(void);
    void deload(void); 
private:
    g2dTexture *ng;
    bool gfBop;
    Anim_OBJECT titleGF;
    std::string funnymessage[2];
    Audio::StreamedFile *freaky; 
    Audio::AudioBuffer *confirm;
    int curStep;
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

class PlayStateScreen : public Screen {
public:
    Character *player;
    Character *opponent;
    Character *gf;
    void load(void); 
    void update(void);
    void draw(void);
    void deload(void);
private:    
    void drawDummyNotes(void);
    void drawNotesAtSection(int sec);
    void drawSustain(int sec, int note, float y, int type);
    void drawNotes(void);
    void updateInput(void);
    g2dTexture *hud;
    Audio::StreamedFile *inst; 
    Audio::StreamedFile *vocals; 
    bool checkPad[4];
    bool checkPadHeld[4];
    noteData chartData; 
    NotePos notePos;
    int curStep;
};

extern Screen *currentScreen;
extern g2dColor screenCol;
void ErrMSG(const char *format, ...);
void setScreen(Screen *scr);
void setScreenCol(g2dColor color);