#include <stdlib.h>
#include <stdio.h>
#include "../game.h"
#include "../error.h"
#include "animation.h"
#include "font.h"

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, int *conf, float speed)
{
	obj->time = 0;
	obj->frames = frames;
	obj->conf = conf;
	obj->speed = speed;
	obj->tick = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
	if (obj->frames[obj->curframe].tex == NULL)
	{
		sprintf(error.message, "ANIMATION DATA IS NULL");
		game.gamestate = 4;
	}
	if (obj->tick)
	{
		obj->time += obj->speed;
		obj->curframe = obj->conf[(int)(obj->time / 100)];
		//if (obj->frames[obj->curframe] == NULL)
		//	obj->curframe -= 1;
	}
	else 
		obj->tick = false;
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
	if (obj->visible)
		DrawG2DTex(obj->frames[obj->curframe].tex, &img, &disp, obj->linear, obj->angle, obj->alpha);
}
