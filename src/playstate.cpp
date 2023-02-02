#include "screen.h"
#include "psp/font.h"
#include "chartparser.h"
const char *song = "bopeebo";
#include "charlist.h"


void PlayStateScreen::load(void)
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
//	PlayStateScreen::opponent->setAnim(IDLE);
//	PlayStateScreen::gf->setAnim(IDLE);

    //load game assets
    sprintf(_path, "assets/songs/%s/%s.json", song, song); //todo implement difficulty
    Parser_loadChart(_path);
    Parser_readInitialData();

    //sprintf(_path, "assets/songs/%s/Inst.wav", song);
    //inst = Audio_LoadSong(_path);
    sprintf(_path, "assets/songs/%s/Vocals.wav", song);
    PlayStateScreen::vocals = Audio_LoadSong(_path);
   	Audio_PlaySong(PlayStateScreen::vocals, false);
}

void PlayStateScreen::update(void)
{
    Parser_tickStep(PlayStateScreen::vocals);
    PlayStateScreen::section = Parser_readChartData(PlayStateScreen::curStep / 16);

   
    PrintFont(Left, 0, 0, "step %d time %d", parser.curStep, parser.songPos);

	//game.player->tick();
//	PlayStateScreen::opponent->tick();
//	PlayStateScreen::gf->tick();

  //  DrawNote(notetex, section.sectionNotes[0], section.sectionNotes[1], section.sectionNotes[2], section.mustHitSection);
 //   DrawDummyNotes(notetex, &test[0]);

}

void PlayStateScreen::draw(void)
{
}

void PlayStateScreen::deload(void)
{
}