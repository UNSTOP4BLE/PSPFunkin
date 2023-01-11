#ifndef __DAD_H__
#define __DAD_H__

#include "../character.h"

extern Character *Dad;

void Dad_SetAnim(int anim);
void Dad_Init(void);
void Dad_Tick(void);
void Dad_FreeChar(void);

#endif