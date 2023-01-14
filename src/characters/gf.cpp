#include "gf.h"
#include "../game.h"
#include "../chartparser.h"

static Character *GF;
static g2dTexture *GF_tex[2];
static bool bop = false;

static AnimFrames gfFrames[] =
{
	//idle left
	{0, 154,   1,  73, 102,  50,  78},
	{0, 228,   1,  73, 101,  50,  77},
	{0, 302,   1,  73, 102,  50,  78}, 
	{0, 376,   1,  79, 105,  52,  81},
	{0,   1, 109,  81, 105,  54,  81},
	//idle right
	{0,  83, 108,  80, 103,  53, 102}, 
	{0, 164, 104,  80, 103,  52, 102},
	{0, 245, 103,  79, 103,  52, 102},
	{0, 325, 106,  74, 104,  46, 103},
};

static int  gfConfIdleL[] = {0,  1,  2,  3,  4};
static int  gfConfIdleR[] = {8,  7,  6,  5,  5};
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
	game.gf = GF;
}

static void GF_SetAnim(CharAnims anim)
{
	switch (anim)
	{
   		case IDLE:
			bop = !bop;
   			if (bop)
	    		AnimOBJECT_SetAnim(&GF->obj, &gfFrames[0], &gfConfIdleR[0], 48, CountOf(gfConfIdleR));
    		else			
    			AnimOBJECT_SetAnim(&GF->obj, &gfFrames[0], &gfConfIdleL[0], 48, CountOf(gfConfIdleL));
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
    	default:
    		break;
	}
}

#include "../psp/font.h"
static void GF_Tick(void)
{
	if (game.justBeat) //switch bop every beat
    	game.gf->setAnim(IDLE);
    PrintMSG(0,40, "%d", bop);
    AnimOBJECT_Tick(&GF->obj);
    AnimOBJECT_Draw(&GF_tex[0], &GF->obj, GF->x, GF->y);
}

static void GF_FreeChar(void)
{
	g2dTexFree(&GF_tex[0]);
	g2dTexFree(&GF_tex[1]);
	delete GF;
}
