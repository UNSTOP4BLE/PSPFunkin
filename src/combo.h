#pragma once
#include "psp/gfx.h"
#include <string>           

#define COMBO_OBJ_MAX 128

class ComboObject
{
public:
    bool tick, falling;
    float x, y;
    int ymin;
    GFX::RECT<int> img;
    GFX::RECT<int> disp;
    int timer;
    float alpha;
private:
};

class Combo
{
public:
    void init(void);
    void spawnNew(std::string name);
    void tick(void);
    void draw(GFX::Texture *tex, float zoom);
    int combo;
private:
    int objectindex;
    ComboObject objects[COMBO_OBJ_MAX];

};