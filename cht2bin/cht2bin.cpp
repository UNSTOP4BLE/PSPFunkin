#include <ostream>  
#include <cstddef>           
#include <cstdio>      
#include <fstream> 
#include <iostream> 
#include <vector>         
#include "json.hpp"                                                                                                                                                                                                                                                                                                                                          

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
    double pos;
    int type;
    double sus; //amon us
    bool isopponent;
};

struct ChartData
{
    double speed;
    double bpm;
    int sectioncount;
    int notecount;
    std::vector<Section> sections;
    std::vector<Note> gamenotes;
};

int main(int argc, char *argv[])
{
    if (argc != 3)
        std::cout << "invalid arguments, usage: ./cht2bin {path_to_chart} {path_to_output}" << std::endl;

    std::ifstream chtfile(argv[1]);
    if (!chtfile.is_open())
    {
        std::cout << "failed to open " << argv[1] << std::endl;
        return 0;
    }
    nlohmann::json chart;
    chtfile >> chart;

    ChartData chartdata;
    chartdata.speed = chart["song"]["speed"];   
    chartdata.bpm = chart["song"]["bpm"];   
    
    //initialize vars
    chartdata.notecount = 1;
    int eventcount = 1;
    chartdata.sections.resize(chart["song"]["notes"].size());
    chartdata.sectioncount = chart["song"]["notes"].size();

    //parse notes and section data
    for (int i = 0; i < chartdata.sectioncount; i++) // i is the current section
    {
        chartdata.notecount += chart["song"]["notes"][i]["sectionNotes"].size()-1;

        for (int j = 0; j < (int)chart["song"]["notes"][i]["sectionNotes"].size(); j++) //copy over all the notes
        {
            Note newnote;

            if ((int)chart["song"]["notes"][i]["sectionNotes"][j][1] == -1) //-1 is for events
            {
                eventcount += 1;
                continue;
            }
            newnote.pos = chart["song"]["notes"][i]["sectionNotes"][j][0]; //note position in ms
            newnote.type = chart["song"]["notes"][i]["sectionNotes"][j][1]; //type
            newnote.sus = chart["song"]["notes"][i]["sectionNotes"][j][2]; //sustain length in ms
            //is note opponent's
            if (!chart["song"]["notes"][i]["mustHitSection"])
                newnote.isopponent = (newnote.type < 4);
            else
                newnote.isopponent = (newnote.type > 3);

            chartdata.gamenotes.push_back(newnote);
        }
        chartdata.sections[i].mustHitSection = chart["song"]["notes"][i]["mustHitSection"]; //is it a opponent section

        bool isalt = chart["song"]["notes"][i]["altAnim"] == true;
        chartdata.sections[i].altAnim = false;
    }

    //write a file       
    FILE *binFile = fopen(argv[2], "wb"); //remember to fclose the file
    if (binFile == NULL) {
        std::cout << "failed to write file " << argv[2] << std::endl;
        return 0;
    }
    fwrite(&chartdata, sizeof(chartdata), sizeof(chartdata), binFile);
    fclose(binFile);   

    std::cout << "found " << eventcount << " events" << std::endl;
    std::cout << "wrote file " << argv[2] << std::endl;

    return 0;
}