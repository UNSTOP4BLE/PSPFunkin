#ifndef __CHARTPARSER_H__
#define __CHARTPARSER_H__

#include <string>          

typedef struct 
{
	double sectionNotes[3];
	int lengthInSteps;
	//int typeOfSection;
	bool mustHitSection;
	//int bpm;
	//bool changeBPM;
	bool altAnim;
} Section;

typedef struct 
{
	int curStep;
	int songTime;
	int noteScroll;
	double initbpm, initspeed;
	int crochet;
	int step_crochet;
} Parser;

extern Parser parser;

void loadChart(const char *filename);
void readInitialData();
Section readChartData(int thesection, int notes);
void readChart(Section *section);

#endif