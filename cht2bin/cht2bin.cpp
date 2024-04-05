#include <ostream>  
#include <cstddef>           
#include <cstdio>      
#include <fstream> 
#include <iostream> 
#include <vector>         
#include "json.hpp"                                                                                                                                                                                                                                                                                                                                          

#define FLAG_SEC_MUSTHIT (1 << 2)
#define FLAG_SEC_ALT     (1 << 3)

#define FLAG_NOTE_HIT        (1 << 2)
#define FLAG_NOTE_ISOPPONENT (1 << 3)

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
    float speed;
    double bpm;
    int32_t sectioncount;
    int32_t notecounto;
    int32_t notecountp;
};

bool compareByPos(const Note &a, const Note &b) {
    return a.pos < b.pos;
}

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
    strncpy(chartdata.magic, "PSPFCHTV1", sizeof(chartdata.magic)); // dont change this
    chartdata.notecounto = 0;
    chartdata.notecountp = 0;
    int eventcount = 0;
    std::vector<Section> sections;
    std::vector<Note> gamenotesopponent;
    std::vector<Note> gamenotesplayer;
    sections.resize(chart["song"]["notes"].size());
    chartdata.sectioncount = chart["song"]["notes"].size();

    //parse notes and section data
    for (int i = 0; i < chartdata.sectioncount; i++) // i is the current section
    {
        sections[i].flag = 0;
        for (int j = 0; j < static_cast<int>(chart["song"]["notes"][i]["sectionNotes"].size()); j++) //copy over all the notes
        {
            Note newnote;
            newnote.flag = 0;

            if (chart["song"]["notes"][i]["mustHitSection"] == true)
                sections[i].flag |= FLAG_SEC_MUSTHIT;

            if (chart["song"]["notes"][i]["altAnim"] == true)
                sections[i].flag |= FLAG_SEC_ALT;

            if (static_cast<int>(chart["song"]["notes"][i]["sectionNotes"][j][1]) == -1) //-1 is for events
            {
                eventcount += 1;
                continue;
            }
            newnote.pos = static_cast<float>(chart["song"]["notes"][i]["sectionNotes"][j][0]); //note position in ms
            newnote.type = static_cast<int32_t>(chart["song"]["notes"][i]["sectionNotes"][j][1]); //type
            newnote.sus = static_cast<float>(chart["song"]["notes"][i]["sectionNotes"][j][2]); //sustain length in ms
            //is note opponent's
            if (sections[i].flag & FLAG_SEC_MUSTHIT && newnote.type > 3)
                newnote.flag |= FLAG_NOTE_ISOPPONENT;    
            else if (!(sections[i].flag & FLAG_SEC_MUSTHIT) && newnote.type < 4)
                newnote.flag |= FLAG_NOTE_ISOPPONENT;    

            if (newnote.type > 3)
                newnote.type -= 4;

            if (newnote.flag & FLAG_NOTE_ISOPPONENT)
                gamenotesopponent.push_back(newnote);
            else
                gamenotesplayer.push_back(newnote);
        }
    }

    //sort notes
    std::sort(gamenotesplayer.begin(), gamenotesplayer.end(), compareByPos); 
    std::sort(gamenotesopponent.begin(), gamenotesopponent.end(), compareByPos); 

    chartdata.notecountp = gamenotesplayer.size();   
    chartdata.notecounto = gamenotesopponent.size();

    //write a file  
    std::ofstream binFile(argv[2], std::ostream::binary);
    if (!binFile.is_open()) {
        std::cout << "failed to write file " << argv[2] << std::endl;
        return 0;
    }
    binFile.write(reinterpret_cast<const char*>(&chartdata), sizeof(chartdata));
    binFile.write(reinterpret_cast<const char*>(sections.data()), sections.size() * sizeof(Section));
    binFile.write(reinterpret_cast<const char*>(gamenotesplayer.data()), gamenotesplayer.size() * sizeof(Note));
    binFile.write(reinterpret_cast<const char*>(gamenotesopponent.data()), gamenotesopponent.size() * sizeof(Note));
    binFile.close();   

    std::cout << "found " << eventcount << " events" << std::endl;
    std::cout << "wrote file " << argv[2] << std::endl;
/*
    //print
    std::cout << "data:" << std::endl;
    for (int i = 0; i < static_cast<int>(chartdata.notecountp); i++) {
        std::cout << "note " << i << std::endl;
        std::cout << "pos " << gamenotesplayer[i].pos << std::endl;
        std::cout << "type " << gamenotesplayer[i].type << std::endl;
        std::cout << "sus " << gamenotesplayer[i].sus << std::endl;
        std::cout << "" << std::endl;
    }
*/

    return 0;
}