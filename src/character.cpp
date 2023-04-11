#include "main.h"
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

#include "psp/font.h"
void Character::tick(void) {
    AnimOBJECT_Tick(&obj);  
    //set animations
    switch (Hash::FromString(type.c_str()))
    {
        case "gf"_h:
            if (app->parser.justStep && !(app->parser.curStep % 4))
            {
                gfBop = !gfBop;
                setAnim(1);
            }
            PrintFont(Left, 0, 20, "%d", obj.curframe);
            break;
        default:
            if (app->parser.justStep && !(app->parser.curStep % 8))
                setAnim(0);
            break;
    }
}

void Character::draw(void) {
    AnimOBJECT_Draw(&obj, x, y);
}
