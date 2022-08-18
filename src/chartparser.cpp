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

Section readChartData(int thesection, int notes)
{
	Section section;
	section.sectionNotes[0] = chart["song"]["notes"][thesection]["sectionNotes"][notes][0].asDouble(); //note position in ms
	section.sectionNotes[1] = chart["song"]["notes"][thesection]["sectionNotes"][notes][1].asDouble(); //type
	section.sectionNotes[2] = chart["song"]["notes"][thesection]["sectionNotes"][notes][2].asDouble(); //sustain length in ms
	section.lengthInSteps = chart["song"]["notes"][thesection]["lengthInSteps"].asInt();
	section.mustHitSection = chart["song"]["notes"][thesection]["mustHitSection"].asBool(); //is it a opponent section
	section.altAnim = chart["song"]["notes"][thesection]["altAnim"].asBool(); //play a alt animation

	return section;
}


#include "psp/font.h"
void readChart(Section *section)
{
	PrintMSG(0, 0, "%f", section->sectionNotes[0]);
}