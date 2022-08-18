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

void loadChart(std::string filename);
void readInitial(int thesection);
void readChartData(Section *section, int thesection, int notes);

#endif