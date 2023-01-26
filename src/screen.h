#pragma once
#include "common.h"

class Screen {
public:
	inline Screen(void) {}
    virtual void load(void) {}
    virtual void update(void) {}
	virtual ~Screen(void) {}
};

class ErrorScreen : public Screen {
public:
    void load(void) {}
    void update(void);
};

class TitleScreen : public Screen {
public:
    void load(void); 
    void update(void);
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
private:	
	Section section;
	Mix_Music *inst;
	Mix_Music *vocals;
	int curStep;
};

extern Screen *currentScreen;

void ErrMSG(const char *format, ...);
void setScreen(Screen *scr);