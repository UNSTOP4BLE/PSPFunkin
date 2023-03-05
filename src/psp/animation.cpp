#include "animation.h"
#include <stdlib.h>
#include <stdio.h>
#include "../screen.h"

void AnimOBJECT_Init(Anim_OBJECT *obj, std::vector<AnimFrames> frames, std::vector<std::vector<int>> conf)
{
    obj->frames = frames;
    obj->conf = conf;
}

void AnimOBJECT_SetAnim(Anim_OBJECT *obj, int anim)
{
    if (obj == NULL)
    {
        ErrMSG("OBJECT IS NULL");   
        return;
    }

    obj->time = 0;
    obj->curframe = 0;
    obj->curanim = anim;
    obj->size = obj->conf[anim].size()-2;
    obj->speed = obj->conf[anim][0];
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
        int frame = (int)(obj->time / 100)+1;
        if (obj->time > 0)
            obj->curframe = obj->conf[obj->curanim][frame];
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
