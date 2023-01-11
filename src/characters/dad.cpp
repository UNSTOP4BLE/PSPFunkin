#include "dad.h"

Character *Dad = new Character();
g2dTexture *Dad_tex[2];

static AnimFrames dadFrames[] =
{
	//idle
	{0, 398, 184, 107, 190,   0,   0},
	{1,   1,   1, 106, 190,   0,   0},
	{1, 108,   1, 106, 190,   0,   0},
	{1, 215,   1, 104, 192,   0,   0},
	{1, 320,   1, 106, 192,   0,   0},
	//left
	{0, 207, 186,  93, 195,   0,   0},
	{0, 301, 186,  96, 195,   0,   0},
	//down
	{0, 232,   1, 118, 183,   0,   0},
	{0, 351,   1, 115, 184,   0,   0},
	//up
	{0,   1, 200, 102, 205,   0,   0},
	{0, 104, 200, 102, 203,   0,   0},
	//right
	{0,   1,   1, 116, 198,   0,   0},
	{0, 118,   1, 113, 198,   0,   0}
};

static int  dadConfIdle[] = { 0,  1,  2,  3,  4};
static int  dadConfLeft[] = { 5,  6};
static int  dadConfDown[] = { 7,  8};
static int    dadConfUp[] = { 9, 10};
static int dadConfRight[] = {11, 12};

void Dad_SetAnim(int anim)
{
	switch (anim)
	{
   		case ANIM_IDLE:
    		AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfIdle[0], 15, CountOf(dadConfIdle));
    		break;
   		case ANIM_LEFT:
    		AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfLeft[0], 15, CountOf(dadConfLeft));
    		break;
   		case ANIM_DOWN:
    		AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfDown[0], 15, CountOf(dadConfDown));
    		break;
   		case ANIM_UP:
    		AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfUp[0], 15, CountOf(dadConfUp));
    		break;
   		case ANIM_RIGHT:
    		AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfRight[0], 15, CountOf(dadConfRight));
    		break;
	}
}

void Dad_Init(void)
{
	Dad_tex[0] = g2dTexLoad("assets/characters/dad/sheet0.png", G2D_SWIZZLE);
	Dad_tex[1] = g2dTexLoad("assets/characters/dad/sheet1.png", G2D_SWIZZLE);

	Dad->setAnim = Dad_SetAnim;
	Dad->tick = Dad_Tick;
	Dad->free = Dad_FreeChar;
}

void Dad_Tick(void)
{
    AnimOBJECT_Tick(&Dad->obj);
    AnimOBJECT_Draw(&Dad_tex[0], &Dad->obj, Dad->x, Dad->y);
}

void Dad_FreeChar(void)
{
	g2dTexFree(&Dad_tex[0]);
	g2dTexFree(&Dad_tex[1]);
	delete Dad;
}
