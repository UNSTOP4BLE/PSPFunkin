#include "character.h"

void Character_isPlayer(Character *chr)
{
	chr->player = !chr->player;

	if (chr->player)
	{
		chr->obj->flipped = true;
	}
}

void Character_setPos(Character *chr, int x, int y)
{
	chr->x = x;
	chr->y = y;
}

void Character_setFocus(Character *chr, int x, int y, int zoom)
{
	chr->camx = x;
	chr->camy = y;
	chr->camzoom = zoom;
}

void Character_setIcon(Character *chr, int i)
{
	chr->icon = i;
}