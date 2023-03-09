#pragma once
#include "glib2d.h"
#include <vector>       
#include <string>

struct AnimFrames
{
    int tex;
    int x, y, w, h, offsetx, offsety;
};

struct Anim_OBJECT
{
    float speed;
    float time;
    float angle;
    int curframe;
    int curanim;
    int size;
    int alpha;
    bool linear;
    bool flipped;
    bool visible;
    bool tick;
    bool cananimate;
    std::vector<AnimFrames> frames;
    std::vector<std::vector<int>> conf;
    std::vector<g2dTexture*> textures;
    
    inline Anim_OBJECT(void)
    {
        speed = 0;
        time = 0;
        angle = 0;
        curframe = 0;
        curanim = 0;
        size = 0;
        alpha = 255;
        linear = false;
        flipped = false;
        visible = true;
        tick = false;
        cananimate = false;
    }

    inline ~Anim_OBJECT(void)
    {
        for (int i = 0; i < (int)textures.size(); i++)
        {
            g2dTexFree(&textures[i]);
        }   
    }
};
void AnimOBJECT_Init(Anim_OBJECT *obj, std::string path, std::string objname);
void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(Anim_OBJECT *obj, int x, int y);
