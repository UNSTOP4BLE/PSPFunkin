#include "game.h"
#include "chartparser.h"
#include "psp/file.h"
#include "psp/audio.h"
#include "psp/pad.h"
#include "psp/font.h"

#include "charlist.h"

Section section;
Mix_Music *inst;
Mix_Music *vocals;
const char *song = "bopeebo";

void PlayState_Init(void)
{
    char _path[40];
    sprintf(_path, "assets/songs/%s/config.json", song);
	Json::Value _config;
	loadJson(_path, &_config);

	//load characters
	setChar(_config["opponent"].asString());
	game.opponent->setAnim(IDLE);

    //load game assets
    sprintf(_path, "assets/songs/%s/%s.json", song, song); //todo implement difficulty
    loadChart(_path);
    readInitialData();

    //sprintf(_path, "assets/songs/%s/Inst.wav", song);
    //inst = Audio_LoadSong(_path);
    sprintf(_path, "assets/songs/%s/Vocals.wav", song);
    vocals = Audio_LoadSong(_path);
   	Audio_PlaySong(vocals, false);
}

void PlayState(void)
{
	game.opponent->tick();
    
    tickStep(vocals);

    PrintMSG(0, 0, "step %d time %f", parser.curStep, parser.songPos);

	if (parser.curStep % 8 == 7)
		game.opponent->setAnim(IDLE);
    
    	section = readChartData(parser.curStep / 16);

  //  DrawNote(notetex, section.sectionNotes[0], section.sectionNotes[1], section.sectionNotes[2], section.mustHitSection);
 //   DrawDummyNotes(notetex, &test[0]);
}