#ifndef __GAME_H__
#define __GAME_H__

typedef struct 
{
	int gamestate = 0;
	double deltaTime = 0;
} Game;

extern Game game;

void PlayState_Init(void);
void PlayState(void);
#endif