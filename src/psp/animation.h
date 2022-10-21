#ifndef __ANIMATION_H__
#define __ANIMATION_H__

typedef struct
{
	int x, y, w, h, offsetx, offsety;
} AnimFrames;

void PlayAnim(g2dTexture* tex, AnimFrames Frames[], float animx, float animy, int animcooldown, int backframe, bool flipanim, int *anim, int *animspeed, int w, int h, bool linear, bool cananimate, float angle);

#endif