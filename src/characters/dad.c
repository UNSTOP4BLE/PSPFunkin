#include "../character.h"

Character Dad;

void Dad_Init()
{
	//load sprites
	Dad.tex[0] = g2dTexLoad("assets/characters/dad/sheet0.png", G2D_SWIZZLE);
	Dad.tex[1] = g2dTexLoad("assets/characters/dad/sheet1.png", G2D_SWIZZLE);
	
}

static AnimFrames dadFrames[] =
{
	//idle
	{Dad.tex[0], 0, 0, 0, 0, 0, 0},
	{Dad.tex[1], 0, 0, 0, 0, 0, 0},
	{Dad.tex[1], 0, 0, 0, 0, 0, 0},
	{Dad.tex[1], 0, 0, 0, 0, 0, 0},
	{Dad.tex[1], 0, 0, 0, 0, 0, 0},
	//left
	{Dad.tex[0], 0, 0, 0, 0, 0, 0},
	{Dad.tex[0], 0, 0, 0, 0, 0, 0},
	//down
	{Dad.tex[0], 0, 0, 0, 0, 0, 0},
	{Dad.tex[0], 0, 0, 0, 0, 0, 0},
	//up
	{Dad.tex[0], 0, 0, 0, 0, 0, 0},
	{Dad.tex[0], 0, 0, 0, 0, 0, 0},
	//right
	{Dad.tex[0], 0, 0, 118, 200, 0, 0},
	{Dad.tex[0], 118, 0, 114, 200, 0, 0}
};

static int dadConf[5][] =
{
	//idle
	{0, 1, 2, 3, 4},
	//left
	{5, 6},
	//down
	{7, 8},
	//up
	{9, 10},
	//right
	{11, 12}
};

void Dad_SetAnim(int anim)
{
    AnimOBJECT_SetAnim(&dad, &dadFrames[0], &dadConf[anim][0], 15, sizeof(dadConf)/sizeof(dadConf[anim][0]));
}

void Dad_Tick(void)
{
    AnimOBJECT_Tick(&dad);
    AnimOBJECT_Draw(&dad, dad.x, dad.y);
}

