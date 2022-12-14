#ifndef __CHARTPARSER_H__
#define __CHARTPARSER_H__

#include <string>       
#include "psp/audio.h"

struct Section
{
	double sectionNotes[3];
	//int typeOfSection;
	bool mustHitSection;
	//int bpm;
	//bool changeBPM;
	bool altAnim;
};

struct Parser
{
	int curStep;
	int songPos;
	double interpPos;
	double initbpm, initspeed;
	double crochet;
	double step_crochet;
};

extern Parser parser;
extern int notecount;

void loadChart(const char *filename);
void readInitialData();
Section readChartData(int thesection);
void tickStep(Mix_Music *song);

#endif