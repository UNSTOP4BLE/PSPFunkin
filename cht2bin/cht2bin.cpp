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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "invalid arguments, usage: ./cht2bin {path_to_chart} {path_to_output}" << std::endl;
        return 0;
    }

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
    chartdata.magic = 42069; // dont change this
    chartdata.notecount = 1;
    int eventcount = 0;
    std::vector<Section> sections;
    std::vector<Note> gamenotes;
    sections.resize(chart["song"]["notes"].size());
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

            gamenotes.push_back(newnote);
        }
        sections[i].mustHitSection = chart["song"]["notes"][i]["mustHitSection"]; //is it a opponent section

        bool isalt = chart["song"]["notes"][i]["altAnim"] == true;
        sections[i].altAnim = false;
    }

    //write a file  
    std::ofstream binFile(argv[2], std::ostream::binary);
    if (!binFile.is_open()) {
        std::cout << "failed to write file " << argv[2] << std::endl;
        return 0;
    }
    binFile.write(reinterpret_cast<const char*>(&chartdata), sizeof(chartdata));
    binFile.write(reinterpret_cast<const char*>(sections.data()), sections.size());
    binFile.write(reinterpret_cast<const char*>(gamenotes.data()), gamenotes.size());
    binFile.close();   

    std::cout << "found " << eventcount << " events" << std::endl;
    std::cout << "wrote file " << argv[2] << std::endl;

    return 0;
}