#pragma once
#include "gfx.h"
#include <vector>       
#include <string>

enum AnimationModes
{ 
    ModeStep, 
    ModeNone 
};

struct AnimFrames
{
    int tex;
    int x, y, w, h, offsetx, offsety;
};

struct Animation
{
    int anim;
    int tex;
    int framecount;
    int endtime;
};

class Anim_OBJECT {
public:
    Anim_OBJECT(void);
    ~Anim_OBJECT(void);
    Animation curanim;
    int curframe;
    double animtime;
    bool tick;
    bool cananimate;
    AnimationModes mode;
    std::vector<AnimFrames> frames;
    std::vector<std::vector<int>> conf;
    std::vector<int> speed;
    std::vector<GFX::Texture*> textures;
};

void AnimOBJECT_Init(Anim_OBJECT *obj, std::string path, std::string objname);
void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim, AnimationModes mode);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(Anim_OBJECT *obj, float x, float y, bool linear, float angle, int alpha, float zoom);