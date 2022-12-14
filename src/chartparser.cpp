#include <fstream>
#include <json/json.h>
#include "chartparser.h"
#include "error.h"
#include "game.h"

Json::Value chart;

Parser parser;

static void LoadJson(std::string filename, Json::Value *data) 
{
    std::ifstream configFile(filename);
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    parseFromStream(builder, configFile, data, &errs);
    configFile.close();
}

void loadChart(const char *filename) 
{
	LoadJson(filename, &chart);
	if (!chart)
	{
		sprintf(message, "FAILED TO FIND CHART: %s", filename);
		game.gamestate = 4;
		return;
	}
}

void readInitialData()
{
	//read initial data from the json
	parser.initspeed = chart["song"]["speed"].asDouble();	
	parser.initbpm = chart["song"]["bpm"].asDouble();	
	parser.crochet = (60.0 / parser.initbpm) * 1000.0;
	parser.step_crochet = parser.crochet / 4;
}

static Section section;
int notecount;

Section readChartData(int thesection)
{
	if (notecount >= (int)chart["song"]["notes"][thesection]["sectionNotes"].size())
		notecount = 0;
	else
	{
		if (parser.songPos + 2000 >= chart["song"]["notes"][thesection]["sectionNotes"][notecount][0].asDouble()
			&& chart["song"]["notes"][thesection]["sectionNotes"][notecount][1].asDouble() != -1)
		{
			section.sectionNotes[0] = chart["song"]["notes"][thesection]["sectionNotes"][notecount][0].asDouble(); //note position in ms
			section.sectionNotes[1] = chart["song"]["notes"][thesection]["sectionNotes"][notecount][1].asInt(); //type
			section.sectionNotes[2] = chart["song"]["notes"][thesection]["sectionNotes"][notecount][2].asDouble(); //sustain length in ms
		}
		notecount ++;
	}
	section.mustHitSection = chart["song"]["notes"][thesection]["mustHitSection"].asBool(); //is it a opponent section
	section.altAnim = chart["song"]["notes"][thesection]["altAnim"].asBool(); //play a alt animation

	return section;
}

void tickStep(Mix_Music *song)
{
	if (Audio_IsPlaying())
	{
		parser.interpPos += 16.666666666666668;
    	parser.songPos = (int)(parser.interpPos + (parser.interpPos - Audio_GetSongMilli(song)));
		//parser.songPos =  Audio_GetSongMilli(song);
	    parser.curStep = (parser.songPos / parser.step_crochet);
	}
}	
