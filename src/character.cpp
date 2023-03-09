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
    switch (Hash::FromString(chardata["type"].asString().c_str())) {
        case "player"_h:
            type = player;
            break;
        case "opponent"_h:
            type = opponent;
            break;
        case "gf"_h:
            type = gf;
            break;
        default:
            ErrMSG("CHARACTER %s HAS A UNKNOWN TYPE %s", objstr.c_str(), chardata[type].asString().c_str());    
            return;
            break;
    }
        
    AnimOBJECT_Init(&obj, path, objstr);
    setAnim(0);
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

void Character::setAnim(int anim) {
    AnimOBJECT_SetAnim(&obj, anim);   
}

void Character::setIcon(int i) {
    icon = i;
}

void Character::tick(void) {
    AnimOBJECT_Tick(&obj);  
    //set animations
    switch (type)
    {
        case gf:
            if (parser.justStep && !(parser.curStep % 4))
            {
                gfBop = !gfBop;
                setAnim(gfBop);
            }
        break;
        default:
            if (parser.justStep && !(parser.curStep % 8))
                setAnim(0);
        break;
    }
}

void Character::draw(void) {
    AnimOBJECT_Draw(&obj, x, y);
}
