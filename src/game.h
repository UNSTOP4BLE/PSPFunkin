#ifndef __GAME_H__
#define __GAME_H__

typedef struct 
{
	int gamestate;
	double deltaTime;
} Game;

extern Game game;

#endif