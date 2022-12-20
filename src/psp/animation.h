#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "glib2d.h"

#define CountOf(x) (sizeof(x) / sizeof(x[0]))
typedef struct
{
	g2dTexture *tex;
	int x, y, w, h, offsetx, offsety = 0;
} AnimFrames;

typedef struct
{
	float speed = 0;
	float time = 0;
	float angle = 0;
	int curframe = 0;
	int *conf;
	int size = 0;
	int alpha = 255;
	bool linear = false;
	bool flipped = false;
	bool visible = true;
	bool tick = false;
	AnimFrames *frames;

} Anim_OBJECT;

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, int *conf, float speed, int size);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(Anim_OBJECT *obj, int x, int y);

#endif