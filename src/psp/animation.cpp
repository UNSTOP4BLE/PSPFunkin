#include "../main.h"
#include "../app.h"
#include "animation.h"
#include <stdlib.h>
#include <stdio.h>
#include "file.h"

Anim_OBJECT::Anim_OBJECT(void) {   
    //reset vars
    cantick = cananimate = false;
    curframe = 0;
    mode = ModeNone;

    curanim.anim = 0;
    curanim.framecount = 0;
    curanim.endtime = 0;
    frame.setValue(0);
    PRINT_ALLOC();
}

Anim_OBJECT::Anim_OBJECT(std::string path, std::string objname)
{   
    std::string _path;

    _path = path + objname;
    const JsonAsset *data = app->assetmanager.get<JsonAsset>(getPath(_path.c_str()).c_str());

    conf.resize(data->value["config"].size());
    for (int i = 0; i < static_cast<int>(data->value["config"].size()); i++)
    {
        speed.push_back(data->value["config"][i][0].asInt()); //speed
        for (int j = 1; j < static_cast<int>(data->value["config"][i].size()); j++) //frames per animation
            conf[i].push_back(data->value["config"][i][j].asInt());
    }
    frames = readFramesFromJson(_path.c_str());
    textures.resize(data->value["textures"].size());

    for (int i = 0; i < static_cast<int>(data->value["textures"].size()); i++)
    {
        _path = path + data->value["textures"][i].asString();    
        textures[i] = app->assetmanager.get<ImageAsset>(getPath(_path.c_str()).c_str());  
    }
    _path = path + objname;
    app->assetmanager.release(data->assetpath.c_str());
    
    //reset vars
    cantick = cananimate = false;
    curframe = 0;
    mode = ModeNone;

    curanim.anim = 0;
    curanim.framecount = 0;
    curanim.endtime = 0;
    frame.setValue(0); 

    PRINT_ALLOC();
}

Anim_OBJECT::~Anim_OBJECT(void)
{
    for (int i = 0; i < static_cast<int>(textures.size()); i++)
    {
        app->assetmanager.release(textures[i]->assetpath.c_str());
    }
    PRINT_FREE();
}

void Anim_OBJECT::setAnim(int anim, AnimationModes mode)
{
    mode = mode;

    curanim.anim = anim;
    curframe = conf[curanim.anim][0];
    curanim.framecount = conf[anim].size();
    cantick = cananimate = true;
    if (speed[curanim.anim] == 0)
        speed[curanim.anim] = 1;
    float maxduration, minduration = 0;
    switch(mode)
    {
        case ModeStep:
            maxduration = 4 * app->parser.stepsPerSecond; //endime * steps per second
            minduration = static_cast<float>(curanim.framecount-1)/static_cast<float>(speed[curanim.anim]);
            curanim.endtime = std::min(maxduration, minduration); //end 4 steps later
            frame.timesource = Chrono;
            break;
        case ModeNone:
            curanim.endtime = static_cast<float>(curanim.framecount-1)/static_cast<float>(speed[curanim.anim]);
            frame.timesource = Chrono;
            break;
        default: break;
    }
    frame.setValue(0, static_cast<float>(curanim.framecount-1), curanim.endtime);
}

void Anim_OBJECT::tick(void)
{
    if (cantick && cananimate)
    {
        if (static_cast<int>(frame.getValue()) < 0)
            return;
        curframe = conf[curanim.anim][static_cast<int>(frame.getValue())];
        if (static_cast<int>(frame.getValue()+1) > curanim.framecount-1) {
            cantick = false;
            return;
        }
    }
}

void Anim_OBJECT::draw(float x, float y, float angle, int alpha, float zoom)
{
    if (cananimate)
    {
        RECT<int> img = {frames[curframe].x,
                            frames[curframe].y,
                            frames[curframe].w,
                            frames[curframe].h};

        RECT<float> disp = {static_cast<float>(x - frames[curframe].offsetx),
                                 static_cast<float>(y - frames[curframe].offsety),
                                 static_cast<float>(frames[curframe].w),
                                 static_cast<float>(frames[curframe].h)};

//        ASSERTFUNC(textures[frames[curframe].tex]->image, "texture is NULL");   
        //GFX::drawTex<float>(&textures[frames[curframe].tex]->image, &img, &disp, angle, alpha, zoom);
    }
}
