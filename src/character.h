#pragma once
#include "psp/animation.h"

class Character
{
public:
    Character(const char *path);
    void setPos(float _x, float _y);
    void setFocus(float x, float y, float zoom);
    void setIcon(int i);
private:
    Anim_OBJECT obj;
    float x, y, camx, camy, camzoom;
    int icon;
    bool player;
};
