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

int notecount;

Section Parser_readChartData(int thesection)
{
	Section section;
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

void Parser_tickStep(Mix_Music *song)
{
	if (Audio_IsPlaying())
	{
    	parser.songPos = Audio_GetSongMilli(song);
	    parser.curStep = (parser.songPos / parser.step_crochet);
	   	parser.curBeat = parser.curStep / 4;
	}
}	

