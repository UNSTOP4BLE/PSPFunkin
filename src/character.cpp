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
    obj->setAnim(0, ModeNone);
    singendtime = 0;
}

GFCharacter::GFCharacter(std::string path, std::string objstr, float _x, float _y) : Character::Character(path, objstr, _x, _y) {
    obj_speaker = new Anim_OBJECT(path, "speaker.json");
    obj_speaker->setAnim(0, ModeNone);
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

    if (app->parser.curStep < 0)
        singendtime = app->parser.curStep - 3;
    else
        singendtime = app->parser.curStep + 3;

    issinging = true;
}

void GFCharacter::setAnim(int anim, AnimationModes mode) {
    obj_speaker->setAnim(0, ModeNone);
    Character::setAnim(anim, mode);
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
    obj->tick();  

    if (singendtime < app->parser.curStep)
        issinging = false;
    //set animations
    if (!issinging)
    {
        if (app->parser.justStep && !(app->parser.curStep % 4))
            setAnim(0, ModeStep);
    }
}

void GFCharacter::tick(void) {
    obj_speaker->tick();  
    obj->tick();  

    if (singendtime < app->parser.curStep)
        issinging = false;
    //set animations
    if (!issinging)
    {
        if (app->parser.justStep && !(app->parser.curStep % 4))
        {
            if (obj->curanim.anim == 1)
                setAnim(0, ModeStep);
            else
                setAnim(1, ModeStep); 
        }    
    }
}

void Character::draw(float cx, float cy, float cz) {
    obj->draw(x-cx, y-cy, 0, 255, cz);
}

void GFCharacter::draw(float cx, float cy, float cz) {
    obj_speaker->draw((x-50)-cx, (y+72)-cy, 0, 255, cz);
    Character::draw(cx, cy, cz);
}

Character::~Character(void) {
    delete obj;
}

GFCharacter::~GFCharacter(void) {
    Character::~Character();
//    delete obj_speaker;
}