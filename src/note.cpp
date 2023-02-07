#include "screen.h"
#include "chartparser.h"

void PlayStateScreen::drawDummyNotes(void) 
{
	for (int i = 0; i < 4; i++)
	{
		Rect img = {1 + (40*i), 1, 39, 39};
		for (int j = 0; j < 8; j++)
		{
			Rect disp = {notePos.player[i].x, notePos.player[i].y, 39, 39};	
			DrawG2DTex(hud, &img, &disp, true, 0, 200);

			disp = {notePos.opponent[i].x, notePos.opponent[i].y, 39, 39};	
			DrawG2DTex(hud, &img, &disp, true, 0, 200);
		}
	}
}

void PlayStateScreen::drawNotes(void) 
{
	int curSec = parser.curStep / 16; 
	//draw the visible notes from the count of notes of the current section
	for (int i = 0; i < chartData.Sections[curSec].notecount; i++)
	{
		int curNotex = notePos.opponent[chartData.Sections[curSec].type[i]].x;
		if (chartData.Sections[curSec].mustHitSection) //if its a players note
			curNotex = notePos.player[chartData.Sections[curSec].type[i] + 4].x;
		float curNotey = (chartData.Sections[curSec].pos[i] - parser.songPos) * parser.initspeed;

		Rect img = {
			1 + (40*chartData.Sections[curSec].type[i]),
			121,
			39,
			39
		};
		
		FRect disp = {
			(float)curNotex,
			curNotey,
			39,
			39
		};
		DrawFG2DTex(hud, &img, &disp, true, 0, 200);
	}
}