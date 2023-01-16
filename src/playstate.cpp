#include "playstate.h"
#include "psp/file.h"
#include "psp/audio.h"
#include "psp/pad.h"
#include "psp/font.h"

#include "charlist.h"

const char *song = "bopeebo";

void PlayState::init(void)
{
    char _path[40];
    sprintf(_path, "assets/songs/%s/config.json", song);
	Json::Value _config;
	loadJson(_path, &_config);

	//load characters
	//setChar(_config["player"].asString());
	setChar(_config["opponent"].asString());
	setChar(_config["gf"].asString());
	//game.player->setAnim(IDLE);
	game.opponent->setAnim(IDLE);
	game.gf->setAnim(IDLE);

    //load game assets
    sprintf(_path, "assets/songs/%s/%s.json", song, song); //todo implement difficulty
    loadChart(_path);
    readInitialData();

    //sprintf(_path, "assets/songs/%s/Inst.wav", song);
    //inst = Audio_LoadSong(_path);
    sprintf(_path, "assets/songs/%s/Vocals.wav", song);
    Playstate::vocals = Audio_LoadSong(_path);
   	Audio_PlaySong(PlayState::vocals, false);
}

void PlayState::update(void)
{
    tickStep(PlayState::vocals);
    PlayState::section = readChartData(PlayState::curStep / 16);

   // PrintMSG(0, 0, "step %d time %f", parser.curStep, parser.songPos);

	//game.player->tick();
	PlayState::opponent->tick();
	PlayState::gf->tick();

  //  DrawNote(notetex, section.sectionNotes[0], section.sectionNotes[1], section.sectionNotes[2], section.mustHitSection);
 //   DrawDummyNotes(notetex, &test[0]);

}