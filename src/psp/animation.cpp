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
}

Anim_OBJECT::Anim_OBJECT(std::string path, std::string objname)
{   
    std::string _path;

    _path = path + objname;
    Json::Value data;
    loadJson(_path.c_str(), &data);

    conf.resize(data["config"].size());
    for (int i = 0; i < static_cast<int>(data["config"].size()); i++)
    {
        speed.push_back(data["config"][i][0].asInt()); //speed
        for (int j = 1; j < static_cast<int>(data["config"][i].size()); j++) //frames per animation
            conf[i].push_back(data["config"][i][j].asInt());
    }
    frames = readFramesFromJson(_path.c_str());
    textures.resize(data["textures"].size());

    for (int i = 0; i < static_cast<int>(data["textures"].size()); i++)
    {
        if (data["textures"].size() == 0)
            return;
        _path = path + data["textures"][i].asString();     
        textures[i] = new GFX::Texture();
        textures[i]->load(_path.c_str());  
    }
    _path = path + objname;
    
    //reset vars
    cantick = cananimate = false;
    curframe = 0;
    mode = ModeNone;

    curanim.anim = 0;
    curanim.framecount = 0;
    curanim.endtime = 0;
    frame.setValue(0); 

    debugLog("animationinit: %s anim %d framecount %d endtime %f", _path.c_str(), curanim.anim, curanim.framecount, curanim.endtime);
    debugLog("animationinit: %s curframe %d cantick %d cananimate %d frame %f", _path.c_str(), curframe, cantick, cananimate, frame.getValue());
}

Anim_OBJECT::~Anim_OBJECT(void)
{
    for (int i = 0; i < static_cast<int>(textures.size()); i++)
    {
        if (textures.size() == 0)
            return;
        delete textures[i];
        textures[i] = nullptr;
    }
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
        GFX::RECT<int> img = {frames[curframe].x,
                            frames[curframe].y,
                            frames[curframe].w,
                            frames[curframe].h};

        GFX::RECT<float> disp = {static_cast<float>(x - frames[curframe].offsetx),
                                 static_cast<float>(y - frames[curframe].offsety),
                                 static_cast<float>(frames[curframe].w),
                                 static_cast<float>(frames[curframe].h)};

        ASSERTFUNC(textures[frames[curframe].tex], "texture is NULL");   
        GFX::drawTex<float>(textures[frames[curframe].tex], &img, &disp, angle, alpha, zoom);
    }
}
