#pragma once
#include "glib2d.h"
#include "types.h"

void AnimOBJECT_Init(Anim_OBJECT *obj, std::vector<AnimFrames> frames, std::vector<std::vector<int>> conf);
void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim, float speed);
void AnimOBJECT_Tick(Anim_OBJECT *obj);
void AnimOBJECT_Draw(g2dTexture **textures, Anim_OBJECT *obj, int x, int y);
