#include "screen.h"
#include "psp/font.h"
#include "chartparser.h"

void TitleScreen::load(void) 
{
	Json::Value menuJson;
	loadJson("assets/songs/freaky/freaky.json", &menuJson);
	setScreenCol(BLACK);
	menutrack = Audio_LoadSong("assets/songs/freaky/freaky.wav");
	Audio_PlaySong(menutrack, true);
	parser.initbpm = menuJson["bpm"].asDouble();	
	Parser_calcCrochet();
//	titleGF = new ANIM_OBJECT();
}

void TitleScreen::update(void) 
{
	Bold_Tick(); //animate bold font
	Parser_tickStep(menutrack);
}

void TitleScreen::draw(void) 
{
	PrintFont(Left, 0, 0, "%d", parser.curStep);
	switch (parser.curBeat) 
	{
		case 3:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, "PRESENT");
		case 2:
		case 1:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, "UNSTOPABLE\nIGORSOU\nMAXDEV\nSPICYJPEG\nBILIOUS");
			break;
		case 7:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, "NEWGROUNDS");
						//draw newgrounds logo
    	case 6:
		case 5:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, "IN ASSOCIATION\nWITH");
			break;

		case 11:
			//funny message 2
		case 10:
		case 9:
			//funny message 1
			break;
					
		case 15:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, "FUNKIN");
		case 14:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, "FRIDAY");
		case 13:
			PrintBOLD(Center, G2D_SCR_W / 2, G2D_SCR_H / 2, "NIGHT");
			break;
	}
}

void TitleScreen::deload(void) 
{
	Audio_FreeSong(menutrack);
}