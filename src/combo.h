#pragma once
#include <string>   
#include "psp/gfx_common.h"
#include "psp/tween.h" 
#include "psp/assetmanager.h"          

#define COMBO_OBJ_MAX 128

class ComboObject
{
public:
    bool tick, falling;
    float x;
    Tween<float, QuadInOutEasing, Chrono> y;
    int ymin;
    Gfx::RectWH<int> img;
    Gfx::RectWH<int> disp;
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
    void draw(const ImageAsset *tex, float zoom);
    int combo;
private:
    int objectindex;
    ComboObject objects[COMBO_OBJ_MAX];

};