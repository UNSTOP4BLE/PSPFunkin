#include "gf.h"
#include "../game.h"

static Character *GF;
static g2dTexture *GF_tex[2];
static bool bop;

static AnimFrames gfFrames[] =
{
	//idle
	{0, 398, 184, 107, 190,  79, 189},
	{1,   1,   1, 106, 190,  78, 189},
	{1, 108,   1, 106, 190,  78, 189},
	{1, 215,   1, 104, 192,  77, 191},
	{1, 320,   1, 106, 192,  77, 191},
};

static int  gfConfIdle[2][] = {
{ 0,  1,  2,  3,  4},
{ 0,  1,  2,  3,  4}
};
static int  gfConfLeft[] = { 5,  6};
static int  gfConfDown[] = { 7,  8};
static int    gfConfUp[] = { 9, 10};
static int gfConfRight[] = {11, 12};

static void GF_SetAnim(CharAnims anim);
static void GF_Tick(void);
static void GF_FreeChar(void);

void GF_Init(void)
{
	GF = new Character();
	GF_tex[0] = g2dTexLoad("assets/characters/gf/sheet0.png", G2D_SWIZZLE);
	GF_tex[1] = g2dTexLoad("assets/characters/gf/sheet1.png", G2D_SWIZZLE);

	GF->setAnim = GF_SetAnim;
	GF->tick = GF_Tick;
	GF->free = GF_FreeChar;
	game.opponent = GF;
}

static void GF_SetAnim(CharAnims anim)
{
	switch (anim)
	{
   		case IDLE:
    		AnimOBJECT_SetAnim(&GF->obj, &gfFrames[0], &gfConfIdle[bop][0], 24, CountOf(gfConfIdle[bop]));
    		break;
   		case LEFT:
    		AnimOBJECT_SetAnim(&GF->obj, &gfFrames[0], &gfConfLeft[0], 24, CountOf(gfConfLeft));
    		break;
   		case DOWN:
    		AnimOBJECT_SetAnim(&GF->obj, &gfFrames[0], &gfConfDown[0], 24, CountOf(gfConfDown));
    		break;
   		case UP:
    		AnimOBJECT_SetAnim(&GF->obj, &gfFrames[0], &gfConfUp[0], 24, CountOf(gfConfUp));
    		break;
   		case RIGHT:
    		AnimOBJECT_SetAnim(&GF->obj, &gfFrames[0], &gfConfRight[0], 24, CountOf(gfConfRight));
    		break;
	}
}

static void GF_Tick(void)
{
	if (parser.curStep % 4 == 3) //switch bop every beat
		bop = !bop;
    AnimOBJECT_Tick(&GF->obj);
    AnimOBJECT_Draw(&GF_tex[0], &GF->obj, GF->x, GF->y);
}

static void GF_FreeChar(void)
{
	g2dTexFree(&GF_tex[0]);
	g2dTexFree(&GF_tex[1]);
	delete GF;
}
