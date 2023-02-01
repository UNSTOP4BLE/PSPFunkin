#pragma once
#include "psp/animation.h"
#include "psp/types.h"

void Character_isPlayer(Character *chr);
void Character_setPos(Character *chr, int x, int y);
void Character_setFocus(Character *chr, int x, int y, int zoom);
void Character_setIcon(Character *chr, int i);