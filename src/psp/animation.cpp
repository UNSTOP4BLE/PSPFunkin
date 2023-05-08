#include "../main.h"
#include "animation.h"
#include <stdlib.h>
#include <stdio.h>

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

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim, int mode)
{
    ASSERTFUNC(obj, "object is null");   

    switch (mode) {
        case 0:
            obj->mustEnd = 0;
            break;
        case 1: 
            obj->mustEnd = app->parser.curStep + 4; //used for gf
            break;
        case 2:
            obj->mustEnd = app->parser.curStep + 8; //used for player or opponent
            break;
        default:
            obj->mustEnd = 0;
            break;
    }
    obj->mode = mode;
    obj->curframe = 0;
    obj->curanim = anim;
    obj->speed = obj->conf[0][0];
    obj->framecount = obj->conf[anim].size();
    obj->tick = true;
    obj->cananimate = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
    ASSERTFUNC(obj, "object is null");   

    if (obj->tick && obj->cananimate)
    {
        obj->tweenframe.setValue(obj->mustEnd, obj->speed);
        if (obj->tweenframe.getValue()+1 > obj->framecount-1)
            obj->cananimate = false;
        obj->curframe = obj->conf[obj->curanim][(int)obj->tweenframe.getValue() + 1];
    }
}

void AnimOBJECT_Draw(Anim_OBJECT *obj, float x, float y, bool linear, float angle, int alpha, float zoom)
{
    if (obj->tick)
    {
        Rect img = {obj->frames[obj->curframe].x,
                    obj->frames[obj->curframe].y,
                    obj->frames[obj->curframe].w,
                    obj->frames[obj->curframe].h};

        FRect disp = {(float)(x - obj->frames[obj->curframe].offsetx),
                      (float)(y - obj->frames[obj->curframe].offsety),
                      obj->frames[obj->curframe].w,
                      obj->frames[obj->curframe].h};
        if (obj->flipped)
            disp.w = -disp.w;

        ASSERTFUNC(obj->textures[obj->frames[obj->curframe].tex], "texture is null");   

        if (obj->visible)
            DrawZoomG2DTex(obj->textures[obj->frames[obj->curframe].tex], &img, &disp, linear, angle, alpha, zoom);
    }
}
