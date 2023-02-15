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
        float curNotey = ((chartData.Sections[sec].pos[i] - parser.songPos) * parser.initspeed) + notePos.opponent[chartData.Sections[sec].type[i]].y;
        if (chartData.Sections[sec].mustHitSection) //if its a players note
        {
            curNotex = notePos.player[chartData.Sections[sec].type[i]].x;
            curNotey = ((chartData.Sections[sec].pos[i] - parser.songPos) * parser.initspeed) + notePos.player[chartData.Sections[sec].type[i]].y;
        }
        if (curNotex+39 >= 0 && curNotex <= G2D_SCR_W && curNotey + 39 >= 0 && curNotey <= G2D_SCR_H)
        {
            Rect img = {
                1 + (40*chartData.Sections[sec].type[i]),
                121,
                39,
                39
            };
            
            FRect disp = {
                (float)curNotex,
                curNotey,
                39,
                39
            };
            DrawFG2DTex(hud, &img, &disp, true, 0, 200);
        }
    }
}
void PlayStateScreen::drawNotes(void) 
{
    int curSec = parser.curStep / 16; 
    //draw the visible notes from the count of notes of the current section
    drawNotesAtSection(curSec);
    //read 1 section ahead for smoother note movement
    if (chartData.Sections[curSec+1].notecount != NULL)
        drawNotesAtSection(curSec+1);
}