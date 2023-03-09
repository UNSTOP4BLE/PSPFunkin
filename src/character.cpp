#include "character.h"
#include "chartparser.h"
#include "psp/file.h"

Character::Character(std::string path, std::string objstr, float _x, float _y) {
    std::string _path = path + objstr;
    Json::Value chardata;
    loadJson(_path.c_str(), &chardata);
    setPos(_x, _y);
    AnimOBJECT_Init(&obj, path, objstr);
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
    if (parser.justStep && !(parser.curStep % 8))
        setAnim(0);
}

void Character::draw(void) {
    AnimOBJECT_Draw(&obj, x, y);
}
