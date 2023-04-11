#pragma once
#include <string>     
#include <json/json.h>
#include "psp/audio_streamed_file.h"
#include "psp/file.h"

struct Note 
{
    bool isevent; //used only by psych engine
    double pos;
    int type;
    double sus; //amon us
    bool isopponent;

    bool hit;
};

struct Section
{
    int notecount;
    //int typeOfSection;
    bool mustHitSection;
    //int bpm;
    //bool changeBPM;   
    bool altAnim;
};

struct ChartData
{
    std::vector<Section> sections;
    std::vector<Note> gamenotes;
    int sectioncount;
};

class ChartParser
{
public:
    void loadChart(const char *filename);
    void calcCrochet(void);
    void readChartData(void);
    void tickStep(Audio::StreamedFile *song);
    int curBeat;
    int curStep;
    int songTime;
    bool justStep;
    double bpm;
    double speed;
    double crochet;
    double step_crochet;
    ChartData chartdata;
private: 
    Json::Value chart;
};