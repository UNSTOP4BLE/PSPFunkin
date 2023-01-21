#ifndef __GAME_H__
#define __GAME_H__

#include "screen.h"
extern double deltaTime;

class GameScreens {
public:
    ErrorScreen     Error;
    TitleScreen     Menu_Title;
    PlayStateScreen Playstate;
};

#endif