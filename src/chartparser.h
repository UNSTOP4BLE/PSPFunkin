#pragma once
#include "main.h"
#include <string>     
#include <json/json.h>
#include "psp/file.h"
#include "psp/audio.h"

struct Note 
{
    bool event; //used only by psych engine
    double pos;
    int type;
    double sus; //amon us
};

struct Section
{
    std::vector<Note> sectionNotes;
    int notecount;
    //int typeOfSection;
    bool mustHitSection;
    //int bpm;
    //bool changeBPM;   
    bool altAnim;
};

struct noteData
{
    std::vector<Section> Sections;
    int sectioncount;
};

struct Parser
{
    int curBeat;
    int curStep;
    int songPos;
    bool justStep;
    double initbpm;
    double initspeed;
    double crochet;
    double step_crochet;
};
extern Parser parser;
extern int notecount;

void Parser_loadChart(const char *filename);
void Parser_readInitialData();
void Parser_calcCrochet();
void Parser_readChartData(noteData &data);
void Parser_tickStep();//Mix_Music *song);