#include "dad.h"
#include "../chartparser.h"

static Character *Dad;
static g2dTexture *Dad_tex[2];

static AnimFrames dadFrames[] =
{
    //idle
};

static int  dadConfIdle[] = { 0,  1,  2,  3,  4};
static int  dadConfLeft[] = { 5,  6};
static int  dadConfDown[] = { 7,  8};
static int  dadConfUp[] = { 9, 10};
static int dadConfRight[] = {11, 12};

static void Dad_SetAnim(CharAnims anim);
static void Dad_Tick(void);
static void Dad_FreeChar(void);

void Dad_Init(void)
{
    Dad = new Character();
    Dad_tex[0] = g2dTexLoad("assets/characters/dad/sheet0.png", G2D_SWIZZLE);
    Dad_tex[1] = g2dTexLoad("assets/characters/dad/sheet1.png", G2D_SWIZZLE);

    Dad->setAnim = Dad_SetAnim;
    Dad->tick = Dad_Tick;
    Dad->free = Dad_FreeChar;
//  game.opponent = Dad;
}

static void Dad_SetAnim(CharAnims anim)
{
    /*
    switch (anim)
    {
        case IDLE:
            AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfIdle[0], 24, CountOf(dadConfIdle));
            break;
        case LEFT:
            AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfLeft[0], 24, CountOf(dadConfLeft));
            break;
        case DOWN:
            AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfDown[0], 24, CountOf(dadConfDown));
            break;
        case UP:
            AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfUp[0], 24, CountOf(dadConfUp));
            break;
        case RIGHT:
            AnimOBJECT_SetAnim(&Dad->obj, &dadFrames[0], &dadConfRight[0], 24, CountOf(dadConfRight));
            break;
        default:
            break;
    }*/
}

static void Dad_Tick(void)
{
//  if (parser.curStep % 8 == 7)
//      game.opponent->setAnim(IDLE);

    AnimOBJECT_Tick(&Dad->obj);
    AnimOBJECT_Draw(&Dad_tex[0], &Dad->obj, Dad->x, Dad->y);
}

static void Dad_FreeChar(void)
{
    g2dTexFree(&Dad_tex[0]);
    g2dTexFree(&Dad_tex[1]);
    delete Dad;
}
