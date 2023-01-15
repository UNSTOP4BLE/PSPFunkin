#ifndef __GAME_H__
#define __GAME_H__

#include "character.h"

struct Game
{
	int gamestate;
	double deltaTime;
	Character *player; 
	Character *opponent; 
	Character *gf; 
};

extern Game game;

void DrawDummyNotes(g2dTexture *note, bool *hitnote);
void DrawNote(g2dTexture *note, double pos, int type, double sus, bool musthit);

void PlayState_Init(void);
void PlayState(void);

#endif