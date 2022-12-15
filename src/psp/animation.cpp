#include <stdlib.h>
#include <stdio.h>
#include "../game.h"
#include "animation.h"

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, float speed)
{
	obj->time = 0;
	obj->frames = frames;
	obj->speed = speed;
	obj->tick = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
	if (obj->tick)
	{
		obj->time += obj->speed;
		obj->curframe = obj->time / 10;
	}
}

void AnimOBJECT_Draw(Anim_OBJECT *obj, int x, int y)
{
	Rect img = {obj->frames[obj->curframe].x,
				obj->frames[obj->curframe].y,
				obj->frames[obj->curframe].w,
				obj->frames[obj->curframe].h};

	Rect disp = {x,
				 y,
				 obj->frames[obj->curframe].w,
				 obj->frames[obj->curframe].h};

	DrawG2DTex(obj->frames[obj->curframe].tex, &img, &disp, obj->linear, obj->angle, obj->alpha);
}
