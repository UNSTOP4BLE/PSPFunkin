#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "glib2d.h"

#define CountOf(x) (sizeof(x) / sizeof(x[0]))

struct AnimFrames
{
	int tex;
	int x, y, w, h, offsetx, offsety;
};

struct Anim_OBJECT
{
	float speed;
	float time;
	float angle;
	int curframe;
	int size;
	int alpha;
	bool linear;
	bool flipped;
	bool visible;
	bool tick;
	AnimFrames *frames;
	int *conf;
	
	inline Anim_OBJECT(void)
	{
		speed = 0;
		time = 0;
		angle = 0;
		curframe = 0;
		size = 0;
		alpha = 255;
		linear = false;
		flipped = false;
		visible = true;
		tick = false;
	}
};

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, int *conf, float speed, int size);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(g2dTexture **textures, Anim_OBJECT *obj, int x, int y);

#endif