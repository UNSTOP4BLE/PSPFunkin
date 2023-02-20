#pragma once

#include "main.h"
#include "psp/types.h"
#include "psp/glib2d.h"
#include "psp/audio.h"

class Screen {
public:
    inline Screen(void) {}
    virtual void load(void) {}
    virtual void update(void) {}
    virtual void draw(void) {}
    virtual void deload(void) {}
    virtual ~Screen(void) {}
};

class ErrorScreen : public Screen {
public:
    void load(void) {}
    void update(void) {}
    void draw(void);
    void deload(void) {}
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
    Mix_Music *menutrack;
    int curStep;
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
    Mix_Music *inst;
    Mix_Music *vocals;
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