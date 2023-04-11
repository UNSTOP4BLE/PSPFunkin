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

void PlayStateScreen::drawSustain(int note, float y, int type) 
{
    int clipheight = 11;
    int length = app->parser.chartdata.gamenotes[note].sus / app->parser.step_crochet;

    Rect img = {
        161 + (14*type),
        18,
        13,
        clipheight
    };
    Pos sustain;
    if (app->parser.chartdata.gamenotes[note].isopponent) 
        sustain = notePos.opponent[type]; 
    else
        sustain = notePos.player[type];

    int xpos = sustain.x + img.w;
    float ypos = y + (img.h*2);

    for (int i = 0; i < (int)((length*3.6) * (app->parser.speed)); i++)
    {
        if (i == (int)((length*3.6) * (app->parser.speed))-1) //draw sustain ends
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
    for (int i = 0; i < (int)app->parser.chartdata.gamenotes.size(); i++)
    {
        if (app->parser.chartdata.gamenotes[i].isevent == true) //dont draw if its a event
            continue;
        int type = app->parser.chartdata.gamenotes[i].type;
        int curNotex;
        float curNotey;

        Pos note;
        if (app->parser.chartdata.gamenotes[i].isopponent)
            note = notePos.opponent[type];
        else
            note = notePos.player[type];

        curNotex = note.x;
        curNotey = ((app->parser.chartdata.gamenotes[i].pos - app->parser.songTime) * (app->parser.speed/3.6)) 
                    + note.y;

        if (curNotey <= 0 && curNotey >= G2D_SCR_H) //dont draw every note possible please, only visible ones
            continue;

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

        if (app->parser.chartdata.gamenotes[i].sus != 0) //check if the note is a sustain
            drawSustain(i, curNotey, type);
        DrawFG2DTex(hud, &img, &disp, true, 0, 200);
    }
}
