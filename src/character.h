#pragma once
#include "psp/animation.h"

enum CharTypes { 
    player,
    opponent,
    gf
};

class Character
{
public:
    Character(std::string path, std::string objstr, float _x, float _y);
    void setPos(float _x, float _y);
    void setFocus(float x, float y, float zoom);
    void setAnim(int anim);
    void setIcon(int i);

    void tick(void);
    void draw(void);
private:
    Anim_OBJECT obj;
    CharTypes type;
    float x, y, camx, camy, camzoom;
    bool gfBop;
    int icon;
};
