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
	g2dTexture *tex;
	Anim_OBJECT obj;
	bool player;
	int icon;
	int x, y;
	int camx, camy, camzoom;
} Character;

void Character_isPlayer(Character *chr);
void Character_setPos(Character *chr, int x, int y);
void Character_setFocus(Character *chr, int x, int y, int zoom);
void Character_setIcon(Character *chr, int i);

#endif