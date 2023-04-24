#pragma once
#include <string>     
#include <fstream>
#include <json/json.h>
#include "psp/audio_streamed_file.h"
#include "psp/file.h"

#define FLAG_SEC_MUSTHIT (1 << 2)
#define FLAG_SEC_ALT     (1 << 3)

#define FLAG_NOTE_ISOPPONENT (1 << 2)

struct [[gnu::packed]] Section
{
    //int typeOfSection;
    //int bpm;
    //bool changeBPM;   
    int32_t flag;
};

struct [[gnu::packed]] Note 
{
    float pos;
    int32_t type;
    float sus; //amon us
    int32_t flag;
};

struct [[gnu::packed]] ChartData 
{
    char magic[16]; //magic string to make sure its the pspfunkin chart, not something else
    double speed;
    double bpm;
    int32_t sectioncount;
    int32_t notecount;
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