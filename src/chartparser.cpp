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

    //read initial data from the chart
    chart.read(reinterpret_cast<char *>(&chartdata), sizeof(chartdata));
    ASSERTFUNC(!strcmp(chartdata.magic, "PSPFCHTV1"), "invalid chart");
    calcCrochet();
    sections.resize(chartdata.sectioncount);
    gamenotesplayer.resize(chartdata.notecountp);
    gamenotesopponent.resize(chartdata.notecounto);
    chart.read(reinterpret_cast<char *>(sections.data()), sections.size() * sizeof(Section));
    chart.read(reinterpret_cast<char *>(gamenotesplayer.data()), gamenotesplayer.size() * sizeof(Note));
    chart.read(reinterpret_cast<char *>(gamenotesopponent.data()), gamenotesopponent.size() * sizeof(Note));
    chart.close();
}

void ChartParser::calcCrochet(void)
{
    crochet = (60 / chartdata.bpm) * 1000;
    step_crochet = crochet / 4;
}

void ChartParser::tickStep(Audio::StreamedFile *song)
{
    if (song->isPlaying())
        songTime = static_cast<int>(song->getChannel().getTime() * 1000);
    int nextstep = (songTime / step_crochet);
    if (nextstep != curStep)
    {
        curStep = nextstep;  
        justStep = true;
    }

    curBeat = curStep / 4;
}   

int getStep(void) {
    return app->parser.curStep;
}