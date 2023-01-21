#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "common.h"

class Screen {
public:
    virtual void init(void) {}
    virtual void update(void) {}
};

class ErrorScreen : public Screen {
public:
    void update(const char *format, ...);
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
#endif