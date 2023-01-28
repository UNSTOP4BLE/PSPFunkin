#pragma once

#include "common.h"

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
	inline ~TitleScreen(void) {
		deload();
	}
private:
	Anim_OBJECT gf_title;
	Mix_Music *menutrack;
	int curStep;
};

#include "character.h"
#include "chartparser.h"
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
	Section section;
	Mix_Music *inst;
	Mix_Music *vocals;
	int curStep;
};

extern Screen *currentScreen;

void ErrMSG(const char *format, ...);
void setScreen(Screen *scr);