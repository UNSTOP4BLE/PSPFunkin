#include "game.h"
#include "chartparser.h"
#include "psp/audio.h"
#include "psp/pad.h"
#include "psp/font.h"

#include "charlist.h"

Section section;
Mix_Music *inst;
Mix_Music *vocals;

void PlayState_Init(void)
{
	//load characters
	setChar("Char_Dad");
	game.opponent->setAnim(ANIM_IDLE);
    //laod game stuff
    loadChart("assets/chart/bopeebo.json");
    readInitialData();

  //  inst = Audio_LoadSong("assets/Inst.wav");
    vocals = Audio_LoadSong("assets/Vocals.wav");
   	Audio_PlaySong(vocals, false);
}

void PlayState(void)
{
	if (Pad_Pressed(PSP_CTRL_UP))
		game.opponent->setAnim(ANIM_IDLE);
    
	game.opponent->tick();
    
    tickStep(vocals);

    PrintMSG(0, 0, "step %d time %d", parser.curStep, parser.songPos);

    section = readChartData(parser.curStep / 16);

  //  DrawNote(notetex, section.sectionNotes[0], section.sectionNotes[1], section.sectionNotes[2], section.mustHitSection);
 //   DrawDummyNotes(notetex, &test[0]);
}