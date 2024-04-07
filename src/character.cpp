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
    obj = new Anim_OBJECT(path, objstr);
    issinging = false;
    singendtime = 0;
    obj_speaker = NULL;
    if (Hash::FromString(type.c_str()) == "gf"_h) {
        obj_speaker = new Anim_OBJECT(path, "speaker.json");
        obj_speaker->setAnim(0, ModeNone);
    }
    obj->setAnim(0, ModeNone);
    singendtime = 0;
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
    obj->setAnim(anim, mode);  
    if (Hash::FromString(type.c_str()) == "gf"_h)
        obj_speaker->setAnim(0, ModeNone);
    
    if (app->parser.curStep < 0)
        singendtime = app->parser.curStep - 3;
    else
        singendtime = app->parser.curStep + 3;

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
        obj_speaker->tick();  
    obj->tick();  

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
                if (app->parser.justStep && !(app->parser.curStep % 4))
                    setAnim(0, ModeStep);
                break;
        }
    }
}

void Character::draw(float cx, float cy, float cz) {
    if (Hash::FromString(type.c_str()) == "gf"_h) 
        obj_speaker->draw((x-50)-cx, (y+72)-cy, 0, 255, cz);

    obj->draw(x-cx, y-cy, 0, 255, cz);
}

Character::~Character() {
    delete obj;
    if (obj_speaker != NULL)
        delete obj_speaker;
}