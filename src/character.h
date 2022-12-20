#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "psp/animation.h"

#define ANIM_IDLE 0
#define ANIM_LEFT 1
#define ANIM_DOWN 2
#define ANIM_UP 3
#define ANIM_RIGHT 4

typedef struct
{
	Anim_OBJECT obj;
	bool player = false;
	int icon = 0;
	int x = 0;
	int y = 0;
	int camx, camy, camzoom = 0;
} Character;

void Character_isPlayer(Character *chr);
void Character_setPos(Character *chr, int x, int y);
void Character_setFocus(Character *chr, int x, int y, int zoom);
void Character_setIcon(Character *chr, int i);

#endif