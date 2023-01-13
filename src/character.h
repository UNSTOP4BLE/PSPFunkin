#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "psp/animation.h"

enum CharAnims 
{   
	IDLE, 
    LEFT, 
   	DOWN,
   	UP,
   	RIGHT
};

struct Character
{
	Anim_OBJECT obj;
	bool player;
	int icon;
	int x;
	int y;
	int camx, camy, camzoom;
	void (*setAnim)(CharAnims anim);
	void (*tick)(void);
	void (*free)(void);

    inline Character(void) {
       	player = false;
		icon = 0;
		x = 0;
		y = 0;
		camx = 0;
		camy = 0;
		camzoom = 0;
    }
};

void Character_isPlayer(Character *chr);
void Character_setPos(Character *chr, int x, int y);
void Character_setFocus(Character *chr, int x, int y, int zoom);
void Character_setIcon(Character *chr, int i);

#endif