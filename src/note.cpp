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
        int curNotex = notePos.opponent[chartData.Sections[sec].type[i]].x;
        float curNotey = ((chartData.Sections[sec].pos[i] - parser.songPos) * (parser.initspeed/3.6))
                           + notePos.opponent[chartData.Sections[sec].type[i]].y;
        
        if (chartData.Sections[sec].mustHitSection) //if its a players note
        {
            curNotex = notePos.player[chartData.Sections[sec].type[i]].x;
            curNotey = ((chartData.Sections[sec].pos[i] - parser.songPos) * (parser.initspeed/3.6))
                         + notePos.player[chartData.Sections[sec].type[i]].y;
        }
        
        Rect img = {
            1 + (40*chartData.Sections[sec].type[i]),
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
        if (chartData.Sections[sec].sus[i] != 0) //check if the note is a sustain
            drawSustain(sec, i, curNotey);
        DrawFG2DTex(hud, &img, &disp, true, 0, 200);
       
    }
}

void PlayStateScreen::drawSustain(int sec, int note, float y) 
{
    int clipheight = 11;
    int length = chartData.Sections[sec].sus[note] / parser.step_crochet;

    Rect img = {
        161 + (14*chartData.Sections[sec].type[note]),
        18,
        13,
        clipheight
    };
    int xpos = notePos.opponent[chartData.Sections[sec].type[note]].x + img.w;
    if (chartData.Sections[sec].mustHitSection) //if its a players note
        xpos = notePos.player[chartData.Sections[sec].type[note]].x + img.w;
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
