#include "main.h"
#include "chartparser.h"

void ChartParser::loadChart(const char *filename) 
{
    loadJson(filename, &chart);

    curStep = 0;
    songTime = 0;
    crochet = 0;
    step_crochet = 0;

    //read initial data from the json
    speed = chart["song"]["speed"].asDouble();   
    bpm = chart["song"]["bpm"].asDouble();   
    calcCrochet();
}

void ChartParser::calcCrochet(void)
{
    crochet = (60 / bpm) * 1000;
    step_crochet = crochet / 4;
}

void ChartParser::readChartData(void)
{
    chartdata.sections.resize(chart["song"]["notes"].size());
    chartdata.sectioncount = (int)chart["song"]["notes"].size();

    //parse notes and section data
    for (int i = 0; i < chartdata.sectioncount; i++) // i is the current section
    {
        chartdata.sections[i].notecount = (int)chart["song"]["notes"][i]["sectionNotes"].size(); //how many notes are in the section, begins with index 1
     
        for (int j = 0; j < chartdata.sections[i].notecount; j++) //copy over all the notes
        {
            Note newnote;
            if (chart["song"]["notes"][i]["sectionNotes"][j][1].asInt() == -1) //-1 is for events
            {
                newnote.isevent = true;
                //put your event parse code here
                continue;
            }

            newnote.isevent = false;
            newnote.pos = chart["song"]["notes"][i]["sectionNotes"][j][0].asDouble(); //note position in ms
            newnote.type = chart["song"]["notes"][i]["sectionNotes"][j][1].asInt(); //type
            newnote.sus = chart["song"]["notes"][i]["sectionNotes"][j][2].asDouble(); //sustain length in ms
            //is note opponent's
            if (!chart["song"]["notes"][i]["mustHitSection"].asBool())
                newnote.isopponent = (newnote.type < 4);
            else
                newnote.isopponent = (newnote.type > 3);

            chartdata.gamenotes.push_back(newnote);
        }
    
        chartdata.sections[i].mustHitSection = chart["song"]["notes"][i]["mustHitSection"].asBool(); //is it a opponent section
        chartdata.sections[i].altAnim = chart["song"]["notes"][i]["altAnim"].asBool(); //play a alt animation
    }
}


void ChartParser::tickStep(Audio::StreamedFile *song)
{
    if (song->isPlaying())
        songTime = song->getChannel().getTime() * 1000;
    int nextstep = (songTime / step_crochet);
    if (nextstep != curStep)
    {
        curStep = nextstep;  
        justStep = true;
    }

    curBeat = curStep / 4;
}   