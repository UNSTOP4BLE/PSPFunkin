#include <fstream>
#include <json/json.h>
#include "chartparser.h"
#include "error.h"
#include "game.h"

double initbpm, initspeed;
double crochet;
double step_crochet;
Json::Value chart;

static void LoadJson(std::string filename, Json::Value *data) 
{
    std::ifstream configFile(filename);
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    parseFromStream(builder, configFile, data, &errs);
    configFile.close();
}

void loadChart(std::string filename) 
{
	LoadJson(filename, &chart);
	if (!chart)
	{
		error.message = "FAILED TO FIND CHART";
		game.gamestate = 4;
	}
}

void readInitial(int thesection)
{
	initspeed = chart["song"]["speed"].asDouble();	
	initbpm = chart["song"]["bpm"].asDouble();	
	crochet = (60.0 / initbpm) * 1000.0;
	step_crochet = crochet / 4;
}

void readChartData(Section *section, int thesection, int notes)
{
	section->sectionNotes[0] = chart["song"]["notes"][thesection]["sectionNotes"][notes][0].asDouble(); //note position in ms
	section->sectionNotes[1] = chart["song"]["notes"][thesection]["sectionNotes"][notes][1].asDouble(); //type
	section->sectionNotes[2] = chart["song"]["notes"][thesection]["sectionNotes"][notes][2].asDouble(); //sustain length in ms
	section->lengthInSteps = chart["song"]["notes"][thesection]["sectionNotes"]["lengthInSteps"].asInt();
	section->mustHitSection = chart["song"]["notes"][thesection]["sectionNotes"]["mustHitSection"].asBool(); //is it a opponent section
	section->altAnim = chart["song"]["notes"][thesection]["altAnim"].asBool(); //play a alt animation
}