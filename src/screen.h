#pragma once
#include "common.h"

class Screen {
public:
    virtual void init(void) {}
    virtual void update(void) {}
};

extern Screen *currentScreen;

class ErrorScreen : public Screen {
public:
	char msg[256];
    void init(void) {}
    void update(void) {
		PrintMSG(0, 0, "ERROR");
		PrintMSG(0, 20, msg);
    }
};

class TitleScreen : public Screen {
public:
    void init(void); 
    void update(void);
};

#include "character.h"
#include "chartparser.h"
class PlayStateScreen : public Screen {
public:
	Character *player;
	Character *opponent;
	Character *gf;
	void init(void); 
    void update(void);
private:	
	Section section;
	Mix_Music *inst;
	Mix_Music *vocals;
	int curStep;
};

class GameScreens {
public:
	ErrorScreen     Error;
    TitleScreen     Menu_Title;
    PlayStateScreen Playstate;
};

extern GameScreens game;

void ErrMSG(const char *format, ...);
void setScreen(Screen scr);