#include <stdlib.h>
#include <stdio.h>
#include "glib2d.h"
#include "animation.h"

static void InitAnim(g2dTexture* tex, AnimFrames Frames, float animx, float animy, bool flipanim, int w, int h, bool linear, float angle)
{
    g2dBeginRects(tex);
	g2dSetCropXY(Frames.x, Frames.y);
	g2dSetCropWH(Frames.w, Frames.h);
	if (flipanim)
	{
		g2dSetScaleWH(-w, h);
		g2dSetCoordXY(-Frames.offsetx + animx, Frames.offsety + animy);
	}
	else
	{
		g2dSetScaleWH(w, h);
		g2dSetCoordXY(Frames.offsetx + animx, Frames.offsety + animy);
	}

	g2dSetTexLinear(linear);
	g2dSetRotationRad(angle);
	g2dAdd();
	g2dEnd();
}

void PlayAnim(g2dTexture* tex, AnimFrames Frames[], float animx, float animy, int animcooldown, int lastframe, int backframe, bool flipanim, int *anim, int *animspeed, int w, int h, bool linear, bool cananimate, float angle)
{
	if (*anim > lastframe - 1) // we do -1 cus the frames are a array
		*anim = (backframe - 1);
	
	if (cananimate)
	    (*animspeed) ++;
	    
	if (*animspeed > animcooldown)
	{
		*anim += 1;
		*animspeed = 0;
	}

	
    InitAnim(tex, Frames[*anim], animx, animy, flipanim, w, h, linear, angle);
}


