#ifndef __CHARTPARSER_H__
#define __CHARTPARSER_H__

#include <string>       
#include "psp/audio.h"

typedef struct 
{
	double sectionNotes[3];
	//int typeOfSection;
	bool mustHitSection;
	//int bpm;
	//bool changeBPM;
	bool altAnim;
} Section;

typedef struct 
{
	int curStep;
	double songPos;
	double initbpm, initspeed;
	double crochet;
	double step_crochet;
} Parser;

extern Parser parser;

void loadChart(const char *filename);
void readInitialData();
Section readChartData(int thesection);
void tickStep(Mix_Music *song);

#endif