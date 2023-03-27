#include "../main.h"
#include "animation.h"
#include <stdlib.h>
#include <stdio.h>
#include "../screen.h"

void AnimOBJECT_Init(Anim_OBJECT *obj, std::string path, std::string objname)
{
    std::string _path;
    _path = path + objname;
    obj->frames = readFramesFromJson(_path.c_str());
    obj->conf = readConfFromJson(_path.c_str());
    Json::Value data;
    loadJson(_path.c_str(), &data);
    obj->textures.resize(data["textures"].size());
    for (int i = 0; i < (int)data["textures"].size(); i++)
    {
        _path = path + data["textures"][i].asString();     
        obj->textures[i] = g2dTexLoad(_path.c_str(), G2D_SWIZZLE);  
    }
}

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim)
{
    ASSERTFUNC(obj);   

    obj->time = 0;
    obj->curframe = 0;
    obj->curanim = anim;
    obj->size = obj->conf[anim].size()-2;
    obj->speed = obj->conf[anim][0];
    obj->tick = true;
    obj->cananimate = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
    ASSERTFUNC(obj);   

    if (obj->tick && obj->cananimate)
    {
        obj->time += obj->speed+getDT();
        int frame = (int)(obj->time / 100)+1;
        if (obj->time > 0)
            obj->curframe = obj->conf[obj->curanim][frame];
        if (frame+1 > obj->size)
            obj->cananimate = false;
    }
}

void AnimOBJECT_Draw(Anim_OBJECT *obj, int x, int y)
{
    if (obj->tick)
    {
        Rect img = {obj->frames[obj->curframe].x,
                    obj->frames[obj->curframe].y,
                    obj->frames[obj->curframe].w,
                    obj->frames[obj->curframe].h};

        Rect disp = {x - obj->frames[obj->curframe].offsetx,
                     y - obj->frames[obj->curframe].offsety,
                     obj->frames[obj->curframe].w,
                     obj->frames[obj->curframe].h};
        if (obj->flipped)
            disp.w = -disp.w;

        ASSERTFUNC(obj->textures[obj->frames[obj->curframe].tex]);   

        if (obj->visible)
            DrawG2DTex(obj->textures[obj->frames[obj->curframe].tex], &img, &disp, obj->linear, obj->angle, obj->alpha);
    }
}
