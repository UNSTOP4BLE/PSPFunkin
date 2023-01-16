#ifndef __PLAYSTATE_H__
#define __PLAYSTATE_H__

#include "screen.h"
#include "character.h"
#include "chartparser.h"

class PlayState : public Screen {
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