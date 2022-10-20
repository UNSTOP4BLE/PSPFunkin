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
		sprintf(error.message, "FAILED TO FIND CHART: %s", filename);
		game.gamestate = 4;
	}
}

void readInitialData()
{
	parser.initspeed = chart["song"]["speed"].asDouble();	
	parser.initbpm = chart["song"]["bpm"].asDouble();	
	parser.crochet = (60.0 / parser.initbpm) * 1000.0;
	parser.step_crochet = parser.crochet / 4;
}

static Section section;
Section readChartData(int thesection)
{
	for (int i = 0; i < (int)chart["song"]["notes"][thesection]["sectionNotes"].size(); i++)
	{
		section.sectionNotes[0] = chart["song"]["notes"][thesection]["sectionNotes"][i][0].asDouble(); //note position in ms
		section.sectionNotes[1] = chart["song"]["notes"][thesection]["sectionNotes"][i][1].asInt(); //type
		section.sectionNotes[2] = chart["song"]["notes"][thesection]["sectionNotes"][i][2].asDouble(); //sustain length in ms
	}
	section.lengthInSteps = chart["song"]["notes"][thesection]["lengthInSteps"].asInt(); //section length in steps
	section.mustHitSection = chart["song"]["notes"][thesection]["mustHitSection"].asBool(); //is it a opponent section
	section.altAnim = chart["song"]["notes"][thesection]["altAnim"].asBool(); //play a alt animation

	return section;
}

void tickStep()
{
    parser.songPos += (0.017) * 1000;
    parser.curStep = parser.songPos / parser.step_crochet;

}