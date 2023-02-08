#include "chartparser.h"

Json::Value chart;

Parser parser;

void Parser_loadChart(const char *filename) 
{
	loadJson(filename, &chart);
}

void Parser_readInitialData()
{
	parser.curStep = 0;
	parser.songPos = 0;
	parser.initbpm = 0;
	parser.initspeed = 0;
	parser.crochet = 0;
	parser.step_crochet = 0;

	//read initial data from the json
	parser.initspeed = chart["song"]["speed"].asDouble();	
	parser.initbpm = chart["song"]["bpm"].asDouble();	
	Parser_calcCrochet();
}

void Parser_calcCrochet()
{
	parser.crochet = (60 / parser.initbpm) * 1000;
	parser.step_crochet = parser.crochet / 4;
}

void Parser_readChartData(noteData &data)
{
	data.Sections.resize(chart["song"]["notes"].size());

	for (int i = 0; i < (int)chart["song"]["notes"].size(); i++) // i is the current section
	{
		data.Sections[i].notecount = (int)chart["song"]["notes"][i]["sectionNotes"].size(); //how many notes are in the section, begins with index 1

		for (int j = 0; j < data.Sections[i].notecount; j++) //copy over all the notes
		{
			data.Sections[i].pos.resize((int)chart["song"]["notes"][i]["sectionNotes"].size());
			data.Sections[i].type.resize((int)chart["song"]["notes"][i]["sectionNotes"].size());
			data.Sections[i].sus.resize((int)chart["song"]["notes"][i]["sectionNotes"].size());
			data.Sections[i].pos[j] = chart["song"]["notes"][i]["sectionNotes"][j][0].asDouble(); //note position in ms
			data.Sections[i].type[j] = chart["song"]["notes"][i]["sectionNotes"][j][1].asInt(); //type
			data.Sections[i].sus[j] = chart["song"]["notes"][i]["sectionNotes"][j][2].asDouble(); //sustain length in ms
		}
	
		data.Sections[i].mustHitSection = chart["song"]["notes"][i]["mustHitSection"].asBool(); //is it a opponent section
		data.Sections[i].altAnim = chart["song"]["notes"][i]["altAnim"].asBool(); //play a alt animation
	}
}


void Parser_tickStep(Mix_Music *song)
{
	if (Audio_IsPlaying())
		parser.songPos = Audio_GetSongMilli(song);
	int nextstep = (parser.songPos / parser.step_crochet);
	if (nextstep != parser.curStep)
	{
		parser.curStep = nextstep;	
		parser.justStep = true;
	}

	parser.curBeat = parser.curStep / 4;

}	

