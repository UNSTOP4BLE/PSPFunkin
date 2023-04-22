
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
    int sectioncount;
    int notecount;
    std::vector<Section> sections;
    std::vector<Note> gamenotes;
};

void calcCrochet(void)
{
    crochet = (60 / bpm) * 1000;
    step_crochet = crochet / 4;
}

int main()
{
    //initialize vars
    chartdata.notecount = 1;
    chartdata.sections.resize(chart["song"]["notes"].size());
    chartdata.sectioncount = (int)chart["song"]["notes"].size();

    //parse notes and section data
    for (int i = 0; i < chartdata.sectioncount; i++) // i is the current section
    {
        chartdata.notecount += (int)chart["song"]["notes"][i]["sectionNotes"].size()-1;

        for (int j = 0; j < chartdata.sections[i].notecount; j++) //copy over all the notes
        {
            Note newnote;
            if (chart["song"]["notes"][i]["sectionNotes"][j][1].asInt() == -1) //-1 is for events
                continue;

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

    //write a file
}