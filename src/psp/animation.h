#pragma once
#include "gfx.h"
#include <vector>       
#include <string>

struct AnimFrames
{
    int tex;
    int x, y, w, h, offsetx, offsety;
};

class Anim_OBJECT {
public:
    float angle;
    int framecount;
    int curanim;
    int curframe;
    int time;
//    Tween<float, LinearEasing> tweenstepframe;
  //  Tween<float, LinearEasing> tweenframe;
    int mustEnd;
    bool visible;
    bool flipped;
    bool tick;
    bool cananimate;
    std::vector<AnimFrames> frames;
    std::vector<std::vector<int>> conf;
    std::vector<float> speed;
    std::vector<GFX::Texture*> textures;
};

void AnimOBJECT_Init(Anim_OBJECT *obj, std::string path, std::string objname);
void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim, int mode);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(Anim_OBJECT *obj, float x, float y, bool linear, float angle, int alpha, float zoom);