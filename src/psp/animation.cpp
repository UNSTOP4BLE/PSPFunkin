#include "../main.h"
#include "animation.h"
#include <stdlib.h>
#include <stdio.h>

void AnimOBJECT_Init(Anim_OBJECT *obj, std::string path, std::string objname)
{
    obj->angle = 0;
    obj->framecount = 0;
    obj->curanim = 0;
    obj->curframe = 0;
    obj->time = 0;  
    obj->tweenstepframe = 0;
    obj->tweenframe = 0;
    obj->mustEnd = 0;
    obj->visible = true;
    obj->flipped = false;
    obj->tick = false;
    obj->cananimate = false;
    std::string _path;

    _path = path + objname;
    Json::Value data;
    loadJson(_path.c_str(), &data);

    obj->conf.resize(data["config"].size());
    for (int i = 0; i < static_cast<int>(data["config"].size()); i++)
    {
        obj->speed.push_back(data["config"][i]["speed"].asInt());
        for (int j = 1; j < static_cast<int>(data["config"][i]["frames"].size()); j++)
            obj->conf[i].push_back(data["config"][i]["frames"][j].asInt());
    }
    obj->frames = readFramesFromJson(_path.c_str());
    obj->textures.resize(data["textures"].size());
    for (int i = 0; i < static_cast<int>(data["textures"].size()); i++)
    {
        _path = path + data["textures"][i].asString();     
        obj->textures[i] = GFX::loadTex(_path.c_str());  
    }
}

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim, int end)
{
    ASSERTFUNC(obj, "object is null");   

    obj->mustEnd = end;

    obj->tweenframe = 0;
    obj->curframe = 0;
    obj->time = 0;
    obj->curanim = anim;
    obj->framecount = obj->conf[anim].size();
    obj->tick = true;
    obj->cananimate = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
    ASSERTFUNC(obj, "object is null");   

    if (obj->tick && obj->cananimate)
    {
        switch (obj->mustEnd)
        {
            case -1: // undefined end, end whenever the animation is over
                obj->tweenframe.setValue(obj->framecount, obj->speed[obj->curanim], app->parser.curStep); // maybe dont make synced to step later on? i will see
                break;
            default: // animation in steps or beats, 4 step = 1 beat
                obj->time += obj->speed[obj->curanim] - app->deltatime;
                break;

        } 

        if (static_cast<int>(obj->tweenframe.getValue()+1) > obj->framecount) {
            obj->cananimate = false;
            return; 
        }
        obj->curframe = obj->conf[obj->curanim][static_cast<int>(obj->tweenframe.getValue())];
    }
}

void AnimOBJECT_Draw(Anim_OBJECT *obj, float x, float y, bool linear, float angle, int alpha, float zoom)
{
    if (obj->tick)
    {
        GFX::RECT<int> img = {obj->frames[obj->curframe].x,
                    obj->frames[obj->curframe].y,
                    obj->frames[obj->curframe].w,
                    obj->frames[obj->curframe].h};

        GFX::RECT<float> disp = {static_cast<float>(x - obj->frames[obj->curframe].offsetx),
                      static_cast<float>(y - obj->frames[obj->curframe].offsety),
                      obj->frames[obj->curframe].w,
                      obj->frames[obj->curframe].h};
        if (obj->flipped)
            disp.w = -disp.w;

        ASSERTFUNC(obj->textures[obj->frames[obj->curframe].tex], "texture is null");   

        if (obj->visible)
            GFX::drawTexZoom<float>(obj->textures[obj->frames[obj->curframe].tex], &img, &disp, linear, angle, alpha, zoom);
    }
}
