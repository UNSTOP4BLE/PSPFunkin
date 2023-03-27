#include "main.h"
#include "screen.h"
#include "chartparser.h"

void PlayStateScreen::drawDummyNotes(void) 
{
    for (int i = 0; i < 4; i++)
    {
        Rect img = {1 + (40*i), 1, 39, 39};
        for (int j = 0; j < 8; j++)
        {
            Rect disp = {notePos.player[i].x, notePos.player[i].y, 39, 39}; 
            DrawG2DTex(hud, &img, &disp, true, 0, 200);

            disp = {notePos.opponent[i].x, notePos.opponent[i].y, 39, 39};  
            DrawG2DTex(hud, &img, &disp, true, 0, 200);
        }
    }
}

void PlayStateScreen::drawNotesAtSection(int sec)
{
    for (int i = 0; i < chartData.Sections[sec].notecount; i++)
    {
        if (chartData.Sections[sec].sectionNotes[i].event == true) //dont draw if its a event
            continue;

        int type = chartData.Sections[sec].sectionNotes[i].type;
        int curNotex;
        float curNotey;

        if (chartData.Sections[sec].mustHitSection) //if its a players note
        {
            curNotex = notePos.player[type].x;
            curNotey = ((chartData.Sections[sec].sectionNotes[i].pos - parser.songPos) * (parser.initspeed/3.6)) 
                        + notePos.player[type].y;
        }
        else
        {
            curNotex = notePos.opponent[type].x;
            curNotey = ((chartData.Sections[sec].sectionNotes[i].pos - parser.songPos) * (parser.initspeed/3.6))
                        + notePos.opponent[type].y;
        }   

        Rect img = {
            1 + (40*type),
            121,
            39,
            39
        };
                
        FRect disp = {
            (float)curNotex,
            curNotey,
            img.w,
            img.h   
        };
        //rewrite 8 note stuff
        if (chartData.Sections[sec].sectionNotes[i].sus != 0) //check if the note is a sustain
            drawSustain(sec, i, curNotey, type);
        DrawFG2DTex(hud, &img, &disp, true, 0, 200);
    }
}

void PlayStateScreen::drawSustain(int sec, int note, float y, int type) 
{
    int clipheight = 11;
    int length = chartData.Sections[sec].sectionNotes[note].sus / parser.step_crochet;

    Rect img = {
        161 + (14*type),
        18,
        13,
        clipheight
    };
    int xpos = notePos.opponent[type].x + img.w;
    if (chartData.Sections[sec].mustHitSection) //if its a players note
        xpos = notePos.player[chartData.Sections[sec].sectionNotes[note].type].x + img.w;
    float ypos = y + (img.h*2);

    for (int i = 0; i < (int)((length*3.6) * (parser.initspeed)); i++)
    {
        if (i == (int)((length*3.6) * (parser.initspeed))-1) //draw sustain ends
        {
            img.y = 1;
            img.h = 16;
        }
        FRect disp = {(float)xpos, ypos + (i*clipheight), img.w, img.h};
        DrawFG2DTex(hud, &img, &disp, false, 0, 200);
    }
}

void PlayStateScreen::drawNotes(void) 
{
    int curSec = parser.curStep / 16; 
    //draw 2 sections worth of notes
    for (int i = 0; i < 2; i++) {
        if (chartData.sectioncount > curSec+i && chartData.Sections[curSec+i].notecount != 0) //check if the section exists
            drawNotesAtSection(curSec+i);
    }

}
