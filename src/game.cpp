#include "game.h"

#include "psp/pad.h"
#include "psp/font.h"
#include "psp/audio.h"
#include "psp/glib2d.h"
#include "chartparser.h"
#include "error.h"

#include "characters/dad.h"

Game game;

int notePos[2][8] = {
	//x
	{27,  67,  107, 147, //opponent	
     294, 334, 374, 414}, //player
	//y
	{14,  14,  14,  14, //opponent	
     14,  14,  14,  14}, //player
};


Section section;
g2dTexture *notetex;
Mix_Music *inst;
Mix_Music *vocals;
void DrawDummyNotes(g2dTexture *note, bool *hitnote);
void DrawNote(g2dTexture *note, double pos, int type, double sus, bool musthit);

void PlayState_Init(void)
{
	//load characters
    Dad_Init();
 // 	Dad_SetAnim(ANIM_IDLE);

    //laod game stuff
    loadChart("assets/chart/bopeebo.json");
    readInitialData();

  //  inst = Audio_LoadSong("assets/Inst.wav");
    vocals = Audio_LoadSong("assets/Vocals.wav");
	notetex = g2dTexLoad("assets/hud.png", G2D_SWIZZLE);
   	Audio_PlaySong(vocals, false);
}

void PlayState(void)
{
	if (Pad_Pressed(PSP_CTRL_UP))
      	Dad_SetAnim(ANIM_UP);
    Dad_Tick();
    
    tickStep(vocals);

    PrintMSG(0, 0, "step %d time %d", parser.curStep, parser.songPos);

    section = readChartData(parser.curStep / 16);

    bool test[4] = {0, 0, 1, 0};
    DrawNote(notetex, section.sectionNotes[0], section.sectionNotes[1], section.sectionNotes[2], section.mustHitSection);
    DrawDummyNotes(notetex, &test[0]);
}

//todo rewrite this shit
double elapsedTime[2][4];
Rect imganim[4];

void DrawDummyNotes(g2dTexture *note, bool *hitnote)
{
//todo rewrite this shit
	Rect img = {0, 1, 39, 39};
	Rect disp = {0, 0, 39, 39};

	bool checkInput[4] = {Pad_Held(PSP_CTRL_LEFT | PSP_CTRL_SQUARE), Pad_Held(PSP_CTRL_DOWN | PSP_CTRL_CROSS | PSP_CTRL_LTRIGGER), Pad_Held(PSP_CTRL_UP | PSP_CTRL_TRIANGLE | PSP_CTRL_RTRIGGER), Pad_Held(PSP_CTRL_RIGHT | PSP_CTRL_CIRCLE)};

	for (int i = 0; i < 8; i++)
	{
		img.x += 1;
		if (i != 0)
			img.x += 39;
		if (i == 4) //loop the image rect back for player notes
			img.x = 1;
		disp.x = notePos[0][i];
		disp.y = notePos[1][i];

		//calculate animation rects
		if (i < 4 && imganim[i].w == 0)
		{
			imganim[i].w = 39;
			imganim[i].h = 39;
			imganim[i].x += 1;
			if (i != 0)
				imganim[i].x += imganim[i-1].x + 39;
		}

		//draw opponent notes
		if (i < 4)
			DrawG2DTex(note, &img, &disp, true, 0, 200);
		else //draw player notes
		{
			if (checkInput[i-4] && !hitnote[i-4]) //play animation of the ghost note
			{
				elapsedTime[0][i-4] += game.deltaTime;
				if (elapsedTime[0][i-4] < 5*game.deltaTime)
					imganim[i-4].y = 41;
				else
					imganim[i-4].y = 81;
				DrawG2DTex(note, &imganim[i-4], &disp, true, 0, 200); //draw ghost note
			}
			else if (checkInput[i-4] && hitnote[i-4]) //play animation of the hit note
			{
				elapsedTime[1][i-4] += game.deltaTime;
				if (elapsedTime[1][i-4] < 3*game.deltaTime)
					imganim[i-4].y = 161;
				else if (elapsedTime[1][i-4] < 4*game.deltaTime)
					imganim[i-4].y = 201;
				else
					imganim[i-4].y = 241;
				DrawG2DTex(note, &imganim[i-4], &disp, true, 0, 200); //draw ghost note
			}
			else
			{
				elapsedTime[0][i-4] = 0;
				elapsedTime[1][i-4] = 0;
				DrawG2DTex(note, &img, &disp, true, 0, 200); //draw normal note
			}
		}
	}
}

void DrawNote(g2dTexture *note, double pos, int type, double sus, bool musthit)
{
//todo rewrite this shit
	Rect notes_img[4] = {  
		{1, 121, 39, 39}, //left
		{41, 121, 39, 39}, //down
		{81, 121, 39, 39}, //up
		{121, 121, 39, 39} //right
	};

	FRect disp = {0, 0, 39, 39};

	if (musthit)
		disp.x = notePos[0][4 + type]; //players note
	else
		disp.x = notePos[0][type];

    disp.y = ((pos - parser.songPos) * parser.initspeed * 0.25);
	DrawFG2DTex(note, &notes_img[type], &disp, true, 0, 200);
}