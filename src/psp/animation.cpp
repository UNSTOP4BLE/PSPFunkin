#include <stdlib.h>
#include <stdio.h>
#include "../game.h"
#include "../error.h"
#include "animation.h"
#include "font.h"

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, int *conf, float speed, int size)
{
	obj->time = 0;
	obj->frames = frames;
	obj->curframe = 0;
	obj->conf = conf;
	obj->size = size-1;
	obj->speed = speed;
	obj->tick = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
	if (obj->frames[obj->curframe].tex == NULL)
	{
		sprintf(error.message, "ANIMATION DATA IS NULL AT FRAME %d", obj->curframe);
		game.gamestate = 4;
	}

	if (obj->tick)
	{
		obj->time += obj->speed;
		if (obj->time > 0)
			obj->curframe = obj->conf[(int)(obj->time / 100)];

		while (obj->curframe > obj->size)
		{
			obj->time -= 100;
		    obj->curframe -= 1;
		    obj->tick = false;
		}
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
	if (obj->flipped)
		disp.w = -disp.w;
	if (obj->visible)
		DrawG2DTex(obj->frames[obj->curframe].tex, &img, &disp, obj->linear, obj->angle, obj->alpha);
}
