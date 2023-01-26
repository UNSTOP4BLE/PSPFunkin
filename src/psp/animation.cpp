#include "animation.h"
#include <stdlib.h>
#include <stdio.h>
#include "../common.h"

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, int *conf, float speed, int size)
{
	if (obj == NULL)
	{
		ErrMSG("OBJECT IS NULL");	
		return;
	}

	obj->time = 0;
	obj->frames = frames;
	obj->curframe = 0;
	obj->conf = conf;
	obj->size = size-1;
	obj->speed = speed;
	obj->tick = true;
	obj->cananimate = true;
}

void AnimOBJECT_Tick(Anim_OBJECT *obj)
{
	if (obj == NULL)
	{
		ErrMSG("OBJECT IS NULL");	
		return;
	}

	if (obj->tick && obj->cananimate)
	{
		obj->time += obj->speed+getDT();
		if (obj->time > 0)
			obj->curframe = obj->conf[(int)(obj->time / 100)];
		int frame = (int)(obj->time / 100);
		if (frame+1 > obj->size)
			obj->cananimate = false;
	}
}

void AnimOBJECT_Draw(g2dTexture **textures, Anim_OBJECT *obj, int x, int y)
{
	if (obj->tick)
	{
		Rect img = {obj->frames[obj->curframe].x,
					obj->frames[obj->curframe].y,
					obj->frames[obj->curframe].w,
					obj->frames[obj->curframe].h};

		Rect disp = {x - obj->frames[obj->curframe].offsetx,
					 y - obj->frames[obj->curframe].offsety,
					 obj->frames[obj->curframe].w,
					 obj->frames[obj->curframe].h};
		if (obj->flipped)
			disp.w = -disp.w;

		if (textures[obj->frames[obj->curframe].tex] == NULL)
		{
			ErrMSG("ANIMATION DATA IS NULL AT FRAME %d", obj->curframe);
			return;	
		}

		if (obj->visible)
			DrawG2DTex(textures[obj->frames[obj->curframe].tex], &img, &disp, obj->linear, obj->angle, obj->alpha);
	}
}
