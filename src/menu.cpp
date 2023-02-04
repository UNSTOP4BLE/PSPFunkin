#include "screen.h"
#include "psp/font.h"
#include "chartparser.h"
#include "psp/animation.h"
#include "psp/pad.h"


g2dTexture *gfTex[4];
AnimFrames gfFrames[] = {
	//left
	{0,   1,   1, 179, 162,   0, 158},
	{0, 181,   1, 181, 162,   0, 158},
	{0,   1, 164, 181, 161,   0, 158},
	{0, 183, 164, 179, 161,   0, 157},
	{0,   1, 326, 179, 162,   0, 158},
	{0, 181, 326, 179, 162,   0, 158},
	{1,   1,   1, 180, 162,   0, 158},
	{1, 182,   1, 180, 165,   0, 161},
	{1,   1, 164, 179, 165,   0, 161},
	{1, 181, 167, 179, 165,   0, 161},

	//right
	{1,   1, 330, 179, 162,   0, 158},
	{1, 181, 333, 180, 162,   0, 158},
	{2,   1,   1, 181, 162,   0, 158},
	{2, 183,   1, 179, 162,   0, 158},
	{2,   1, 164, 179, 162,   0, 158},
	{2, 181, 164, 179, 163,   0, 158},
	{2,   1, 327, 179, 163,   0, 159},
	{2, 181, 328, 180, 164,   0, 160},
	{3,   1,   1, 179, 164,   0, 160},
	{3, 181,   1, 179, 164,   0, 160}
};

static int gfConfIdleL[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9};
static int gfConfIdleR[] = {10, 11, 12, 13, 14, 15, 16, 17, 19};

void TitleScreen::load(void) 
{
	setScreenCol(BLACK);

	//load menu jsons
	Json::Value titleJson;
	loadJson("assets/menu/title/title.json", &titleJson);
	
	int curmsg = rand() % (titleJson["messages"].size()-1); //get a random message
	funnymessage[0] = titleJson["messages"][curmsg][0].asString();
	funnymessage[1] = titleJson["messages"][curmsg][1].asString();

	//load and play music
	menutrack = Audio_LoadSong("assets/songs/freaky/freaky.wav");
	Audio_PlaySong(menutrack, true);
	parser.initbpm = titleJson["menuSongBPM"].asDouble();	
	Parser_calcCrochet();

	//load textures
	gfTex[0] = g2dTexLoad("assets/menu/title/gf/sheet0.png", G2D_SWIZZLE);
	gfTex[1] = g2dTexLoad("assets/menu/title/gf/sheet1.png", G2D_SWIZZLE);
	gfTex[2] = g2dTexLoad("assets/menu/title/gf/sheet2.png", G2D_SWIZZLE);
	gfTex[3] = g2dTexLoad("assets/menu/title/gf/sheet3.png", G2D_SWIZZLE);

	ng = g2dTexLoad("assets/menu/title/ng.png", G2D_SWIZZLE);
}

void TitleScreen::update(void) 
{
	parser.justStep = false;
	Parser_tickStep(menutrack);
	Bold_Tick(); //animate bold font

	if (parser.justStep && !(parser.curStep % 4))
	{
		gfBop = !gfBop;

		if (gfBop)
			AnimOBJECT_SetAnim((Anim_OBJECT *)&titleGF, &gfFrames[0], &gfConfIdleL[0], 48, CountOf(gfConfIdleL));
		else
			AnimOBJECT_SetAnim((Anim_OBJECT *)&titleGF, &gfFrames[0], &gfConfIdleR[0], 48, CountOf(gfConfIdleR));
	}
    AnimOBJECT_Tick((Anim_OBJECT *)&titleGF);
	if (Pad_Pressed(PSP_CTRL_CROSS | PSP_CTRL_START))
		setScreen(new PlayStateScreen());
}

void TitleScreen::draw(void) 
{
	//NG logo 
	Rect NG_img = {0, 0, 90, 88};
	Rect NG_disp = {G2D_SCR_W / 2 - 90/2, (G2D_SCR_H / 2 + 88/2) - 20, 90, 88};
	switch (parser.curBeat) 
	{
		case 3:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 + 76, "PRESENT");
		case 2:
		case 1:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 114, "UNSTOPABLE");
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 76,  "IGORSOU");
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 38,  "MAXDEV");
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2,       "SPICYJPEG");
		 	PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 + 38,  "BILIOUS");
			break;
		case 7:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 20, "NEWGROUNDS");
			DrawG2DTex(ng, &NG_img, &NG_disp, false, 0, 255);
    	case 6:
		case 5:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 96, "IN ASSOCIATION");
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 58, "WITH");
			break;

		case 11:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, funnymessage[1].c_str());
		case 10:
		case 9:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 38, funnymessage[0].c_str());
			break;
		case 15:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 + 18, "FUNKIN");
		case 14:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 20, "NIGHT");
		case 13:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2 - 58, "FRIDAY");
			break;
	}
    AnimOBJECT_Draw(&gfTex[0], &titleGF, 0, 100);
}

void TitleScreen::deload(void) 
{
	Audio_FreeSong(menutrack);
	g2dTexFree(&ng);
	for (unsigned int i = 0; i < CountOf(gfTex); i++)
		g2dTexFree(&gfTex[i]);
}