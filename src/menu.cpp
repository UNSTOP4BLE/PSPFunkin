#include "screen.h"

void TitleScreen::load(void) 
{
	menutrack = Audio_LoadSong("assets/songs/freaky/freaky.wav");
	Audio_PlaySong(menutrack, true);
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