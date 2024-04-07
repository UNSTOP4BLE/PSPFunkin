#include "main.h"
#include "app.h"
#include "chartparser.h"

void ChartParser::loadChart(const char *filename) 
{
    chart.open(filename, std::istream::binary);
    ASSERTFUNC(chart, "failed to open chart");
    curStep = 0;
    songTime = 0;
    crochet = 0;
    step_crochet = 0;
    stepsPerSecond = 0;

    //read initial data from the chart
    chart.read(reinterpret_cast<char *>(&chartdata), sizeof(chartdata));
    ASSERTFUNC(!strcmp(chartdata.magic, "PSPFCHTV1"), "invalid chart");
    calcCrochet();
    sections.resize(chartdata.sectioncount);
    gamenotes[0].resize(chartdata.notecountp); //player
    gamenotes[1].resize(chartdata.notecounto); //opponent
    chart.read(reinterpret_cast<char *>(sections.data()), sections.size() * sizeof(Section));
    chart.read(reinterpret_cast<char *>(gamenotes[0].data()), gamenotes[0].size() * sizeof(Note)); // player notes
    chart.read(reinterpret_cast<char *>(gamenotes[1].data()), gamenotes[1].size() * sizeof(Note)); // opponent
    chart.close();
}

void ChartParser::calcCrochet(void)
{
    crochet = (60 / chartdata.bpm) * 1000;
    step_crochet = crochet / 4;
    stepsPerSecond = 15/app->parser.chartdata.bpm;
}

void ChartParser::tickStep(Audio::StreamedFile *song)
{   
    if (song->isPlaying())
    {
        songTime = static_cast<int>(song->getChannel().getTime() * 1000);
        
        if (songTime < 0)
            songTime = 0;
    }
    int nextstep = (songTime / step_crochet);

    if (nextstep != curStep)
    {
        curStep = nextstep;  
        curBeat = curStep / 4;
        justStep = true;
    }
}   