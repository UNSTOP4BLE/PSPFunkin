#include "main.h"
#include "chartparser.h"

void ChartParser::loadChart(const char *filename) 
{
    chart.open(filename, std::ostream::binary);
    ASSERTFUNC(chart, "failed to open chart");
    curStep = 0;
    songTime = 0;
    crochet = 0;
    step_crochet = 0;

    //read initial data from the json
    chart.read(reinterpret_cast<char *>(&chartdata), sizeof(chartdata));
    ASSERTFUNC(chartdata.magic == 42069, "invalid chart");
    calcCrochet();
}

void ChartParser::calcCrochet(void)
{
    crochet = (60 / bpm) * 1000;
    step_crochet = crochet / 4;
}

void ChartParser::readChartData(void)
{
    sections.resize(chartdata.sectioncount);
    gamenotes.resize(chartdata.notecount);

    chart.read(reinterpret_cast<char *>(sections.data()), chartdata.sectioncount);
    chart.read(reinterpret_cast<char *>(gamenotes.data()), chartdata.notecount);
}

void ChartParser::tickStep(Audio::StreamedFile *song)
{
    if (song->isPlaying())
        songTime = (int)(song->getChannel().getTime() * 1000);
    int nextstep = (songTime / step_crochet);
    if (nextstep != curStep)
    {
        curStep = nextstep;  
        justStep = true;
    }

    curBeat = curStep / 4;
}   

void ChartParser::closeChart(void)
{
    chart.close();
}
