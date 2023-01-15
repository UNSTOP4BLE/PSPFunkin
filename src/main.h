#ifndef __MAIN_H__
#define __MAIN_H__

//states
#include "playstate.h"
class Screen {
public:
    double deltaTime;
    virtual void init(void) {}
    virtual void update(void) {}
};

class Game {
    public:
    
    PlayState Playstate;

};  
#endif