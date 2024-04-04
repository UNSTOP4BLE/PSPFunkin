#include "main.h"
#include "app.h"
#include "character.h"
#include "chartparser.h"
#include "psp/file.h"
#include "psp/hash.h"
#include "screen.h"

Character::Character(std::string path, std::string objstr, float _x, float _y) {
    std::string _path = path + objstr;
    Json::Value chardata;
    loadJson(_path.c_str(), &chardata);
    setPos(_x, _y);
    setIcon(chardata["icon"].asInt());
    type = chardata["type"].asString();
    obj = new Anim_OBJECT();
    issinging = false;
    AnimOBJECT_Init(obj, path, objstr);
    obj_speaker = NULL;
    if (Hash::FromString(type.c_str()) == "gf"_h) {
        obj_speaker = new Anim_OBJECT();
        AnimOBJECT_Init(obj_speaker, path, "speaker.json");
        AnimOBJECT_SetAnim(obj_speaker, 0, ModeNone);
    }
    setAnim(0, ModeNone);
  //  issinging = false;
//    singendtime = 0;
}

void Character::setPos(float _x, float _y) {
    x = _x;
    y = _y;
}

void Character::setFocus(float x, float y, float zoom) {
    camx = x;
    camy = y;
    camzoom = zoom;
}

void Character::setAnim(int anim, AnimationModes mode) {
    AnimOBJECT_SetAnim(obj, anim, mode);   
    switch (Hash::FromString(type.c_str()))
    {
        case "gf"_h:
                AnimOBJECT_SetAnim(obj_speaker, 0, ModeStep);   
                singendtime = app->parser.curStep + 3;
            break;
        default:
                singendtime = app->parser.curStep + 7;
            break;
    }
    issinging = true;
}

void Character::setIcon(int i) {
    icon = i;
}

int Character::getAnim(void) {
    return obj->curanim.anim;    
}

int Character::getFrame(void) {
    return obj->curframe;    
}

void Character::tick(void) {
    if (Hash::FromString(type.c_str()) == "gf"_h) 
        AnimOBJECT_Tick(obj_speaker);  
    AnimOBJECT_Tick(obj);  

    if (singendtime < app->parser.curStep)
        issinging = false;
    //set animations
    if (!issinging)
    {
        switch (Hash::FromString(type.c_str()))
        {
            case "gf"_h:
                if (app->parser.justStep && !(app->parser.curStep % 4))
                {
                    if (obj->curanim.anim == 1)
                        setAnim(0, ModeStep);
                    else
                        setAnim(1, ModeStep); 
                }
                break;
            default:
                if (app->parser.justStep && !(app->parser.curStep % 8))
                    setAnim(0, ModeStep);
                break;
        }
    }
}

void Character::draw(float cx, float cy, float cz) {
    if (Hash::FromString(type.c_str()) == "gf"_h) 
        AnimOBJECT_Draw(obj_speaker, (x-50)-cx, (y+72)-cy, false, 0, 255, cz);

    AnimOBJECT_Draw(obj, x-cx, y-cy, false, 0, 255, cz);
}

Character::~Character() {
    delete obj;
    if (obj_speaker != NULL)
        delete obj_speaker;
}