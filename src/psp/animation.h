#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "glib2d.h"

typedef struct
{
	g2dTexture *tex;
	int x, y, w, h, offsetx, offsety;
} AnimFrames;

typedef struct
{
	float speed, time;
	float angle;
	int curframe;
	int alpha = 255;
	bool linear;
	bool tick;
	AnimFrames *frames;

} Anim_OBJECT;

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, float speed);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(Anim_OBJECT *obj, int x, int y);

#endif