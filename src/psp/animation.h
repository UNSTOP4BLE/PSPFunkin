#pragma once
#include "gfx.h"
#include <vector>       

#include "assetmanager.h"
#include <string>
#include "tween.h"

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
    int framecount;
    float endtime;
};

class Anim_OBJECT {
public:
    Anim_OBJECT(void);
    Anim_OBJECT(std::string path, std::string objname);
    ~Anim_OBJECT(void);
    void setAnim(int anim, AnimationModes mode);
    void tick(void);
    void draw(float x, float y, float angle, int alpha, float zoom);
    Animation curanim;
    int curframe;
    bool cantick;
    bool cananimate;
    Tween<float, LinearEasing, Chrono> frame;
    AnimationModes mode;
    std::vector<AnimFrames> frames;
    std::vector<std::vector<int>> conf;
    std::vector<int> speed;
    std::vector<const ImageAsset *> textures;
};
