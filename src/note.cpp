#include "screen.h"

void PlayStateScreen::drawDummyNotes(void) 
{
	for (int i = 0; i < 4; i++)
	{
		Rect img = {1 + (40*i), 1, 39, 39};
		for (int j = 0; j < 8; j++)
		{
			Rect disp = {notePos.player[i].x, notePos.player[i].y, 39, 39};	
			DrawG2DTex(hud, &img, &disp, false, 0, 200);

			disp = {notePos.opponent[i].x, notePos.opponent[i].y, 39, 39};	
			DrawG2DTex(hud, &img, &disp, false, 0, 200);
		}
	}
}
/*
void PlayStateScreen::drawNotes(void) 
{
	for (int i = 0; i < 4; i++)
	{
		Rect img = {1 + (40*i), 1, 39, 39};
		for (int j = 0; j < 8; j++)
		{
			Rect disp = {notePos.player[i].x, notePos.player[i].y, 39, 39};	
			DrawG2DTex(hud, &img, &disp, false, 0, 200);

			disp = {notePos.opponent[i].x, notePos.opponent[i].y, 39, 39};	
			DrawG2DTex(hud, &img, &disp, false, 0, 200);
		}
	}

void DrawNote(g2dTexture *note, double pos, int type, double sus, bool musthit)
{
	Rect notes_img[4] = {  
		{1, 1, 39, 39}, //left
		{41, 1, 39, 39}, //down
		{81, 1, 39, 39}, //up
		{121, 1, 39, 39} //right
	};

	Rect disp = {0, 0, 39, 39};

	if (musthit)
		disp.x = notePos[0][4 + type];
	else
		disp.x = notePos[0][type];

	disp.y = (parser.songPos - pos) / parser.initspeed;
	DrawG2DTex(note, &notes_img[type], &disp, true, 0, 200);
}

}*/