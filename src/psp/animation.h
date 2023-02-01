#pragma once
#include "glib2d.h"

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, AnimFrames *frames, int *conf, float speed, int size);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(g2dTexture **textures, Anim_OBJECT *obj, int x, int y);
