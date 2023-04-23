#pragma once
#include <string>     
#include <fstream>
#include <json/json.h>
#include "psp/audio_streamed_file.h"
#include "psp/file.h"

struct Section
{
    //int typeOfSection;
    bool mustHitSection;
    //int bpm;
    //bool changeBPM;   
    bool altAnim;
};

struct Note 
{
    float pos;
    int type;
    float sus; //amon us
    bool isopponent;
};

struct [[gnu::packed]] ChartData {
    int magic; //magic number to make sure its the pspfunkin chart, not something else
    double speed;
    double bpm;
    int sectioncount;
    int notecount;
};

class ChartParser
{
public:
    void loadChart(const char *filename);
    void calcCrochet(void);
    void readChartData(void);
    void tickStep(Audio::StreamedFile *song);
    void closeChart(void);
    int curBeat;
    int curStep;
    int songTime;
    bool justStep;
    double bpm;
    double speed;
    double step_crochet;
    ChartData chartdata;
    std::vector<Section> sections;
    std::vector<Note> gamenotes;
private: 
    std::ifstream chart;
    double crochet;
};