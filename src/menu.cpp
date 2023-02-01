#include "screen.h"

void TitleScreen::load(void) 
{
	setScreenCol(BLACK);
	menutrack = Audio_LoadSong("assets/songs/freaky/freaky.wav");
	Audio_PlaySong(menutrack, true);
//	titleGF = new ANIM_OBJECT();
}

void TitleScreen::update(void) 
{

}

void TitleScreen::draw(void) 
{

}

void TitleScreen::deload(void) 
{
	Audio_FreeSong(menutrack);
}