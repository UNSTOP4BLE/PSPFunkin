#include "character.h"
#include "psp/file.h"

Character::Character(const char *path) {
    Json::Value chardata;
    loadJson(path, &chardata);
    AnimOBJECT_Init(&obj, readFramesFromJson(path), readConfFromJson(path));
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

void Character::setIcon(int i) {
    icon = i;
}
