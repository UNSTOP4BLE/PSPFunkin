#include "../main.h"
#include "../app.h"
#include "animation.h"
#include <stdlib.h>
#include <stdio.h>
#include "file.h"

Anim_OBJECT::Anim_OBJECT(void)
{   

}

Anim_OBJECT::~Anim_OBJECT(void)
{
    for (int i = 0; i < textures.size(); i++)
    {
        GFX::freeTex(textures[i]);
    }
}

void AnimOBJECT_Init(Anim_OBJECT *obj, std::string path, std::string objname)
{
    std::string _path;

    _path = path + objname;
    Json::Value data;
    loadJson(_path.c_str(), &data);

    obj->conf.resize(data["config"].size());
    for (int i = 0; i < static_cast<int>(data["config"].size()); i++)
    {
        obj->speed.push_back(data["config"][i][0].asInt()); //speed
        for (int j = 1; j < static_cast<int>(data["config"][i].size()); j++) //frames per animation
            obj->conf[i].push_back(data["config"][i][j].asInt());
    }
    obj->frames = readFramesFromJson(_path.c_str());
    obj->textures.resize(data["textures"].size());
    for (int i = 0; i < static_cast<int>(data["textures"].size()); i++)
    {
        _path = path + data["textures"][i].asString();     
        obj->textures[i] = GFX::loadTex(_path.c_str());  
    }

    //reset vars

    obj->tick = obj->cananimate = false;
    obj->curframe = 0;
    obj->mode = ModeNone;

    obj->curanim.anim = 0;
    obj->curanim.tex = 0;
    obj->curanim.framecount = 0;
    obj->curanim.endtime = 0;
}

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim, AnimationModes mode)
{
    ASSERTFUNC(obj, "object is null");   

    obj->mode = mode;
    switch(mode)
    {
        case ModeStep:
            obj->curanim.endtime = app->parser.curStep + 4; //end 4 steps later
            break;
        case ModeNone:
            obj->curanim.endtime = 0;
            break;
        default: break;
    }

    obj->curanim.anim = anim;
    obj->curframe = obj->conf[obj->curanim.anim][0];
    obj->curanim.tex = obj->frames[obj->curframe].tex;
    obj->curanim.framecount = obj->conf[anim].size();
    obj->tick = obj->cananimate = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
    ASSERTFUNC(obj, "object is null");   

    if (obj->tick && obj->cananimate)
    {
        switch (obj->mode)
        {
            case ModeStep:
//                obj->tweenframe.setValue(obj->framecount, obj->speed[obj->curanim], app->parser.curStep); // maybe dont make synced to step later on? i will see
                break;
            case ModeNone:
                obj->animtime += 1 + app->deltatime;
                break;
            default: break;
        }

        int frame = 0;

        if (obj->speed[obj->curanim.anim] != 0)
            frame = static_cast<int>(obj->animtime/obj->speed[obj->curanim.anim]);
  
        if (frame+1 > obj->curanim.framecount) {
           obj->tick = false;
           obj->animtime = 0;
           return;
        }
        obj->curframe = obj->conf[obj->curanim.anim][frame];
        obj->curanim.tex = obj->frames[obj->curframe].tex;
    }
    
}

void AnimOBJECT_Draw(Anim_OBJECT *obj, float x, float y, bool linear, float angle, int alpha, float zoom)
{
    ASSERTFUNC(obj, "object is null");   
    if (obj->cananimate)
    {
        GFX::RECT<int> img = {obj->frames[obj->curframe].x,
                            obj->frames[obj->curframe].y,
                            obj->frames[obj->curframe].w,
                            obj->frames[obj->curframe].h};

        GFX::RECT<float> disp = {static_cast<float>(x - obj->frames[obj->curframe].offsetx),
                                 static_cast<float>(y - obj->frames[obj->curframe].offsety),
                                 static_cast<float>(obj->frames[obj->curframe].w),
                                 static_cast<float>(obj->frames[obj->curframe].h)};
  //      if (obj->flipped)
//            disp.w = -disp.w;

        ASSERTFUNC(obj->textures[obj->curanim.tex], "texture is null");   

//        if (obj->visible)
        GFX::drawTexZoom<float>(obj->textures[obj->curanim.tex], &img, &disp, linear, angle, alpha, zoom);
    }
}
