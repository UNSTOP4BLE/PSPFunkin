#pragma once
#include "psp/animation.h"

class Character
{
public:
    Character(std::string path, std::string objstr);
    void setPos(float _x, float _y);
    void setFocus(float x, float y, float zoom);
    void setAnim(int anim);
    void setIcon(int i);

    void tick(void);
    void draw(void);
private:
    Anim_OBJECT obj;
    float x, y, camx, camy, camzoom;
    int icon;
    bool player;
};
