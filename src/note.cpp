#include "main.h"
#include "screen.h"
#include "chartparser.h"

void PlayStateScreen::drawDummyNotes(void) 
{
    for (int i = 0; i < 4; i++)
    {
        GFX::RECT<int> img = {1 + (40*i), 1, 39, 39};
        for (int j = 0; j < 8; j++)
        {
            GFX::RECT<float> disp = {static_cast<float>(notePos.player[i].x - hudcam.camx.getValue()), static_cast<float>(notePos.player[i].y - hudcam.camy.getValue()), 39, 39}; 
            GFX::drawTexZoom<float>(hud, &img, &disp, true, 0, 200, hudcam.zoom.getValue());

            disp = {static_cast<float>(notePos.opponent[i].x - hudcam.camx.getValue()), static_cast<float>(notePos.opponent[i].y - hudcam.camy.getValue()), 39, 39};  
            GFX::drawTexZoom<float>(hud, &img, &disp, true, 0, 200, hudcam.zoom.getValue());
        }
    }
}

void PlayStateScreen::drawSustain(int note, float y, int type) 
{
    int clipheight = 11;
    int length = app->parser.gamenotes[note].sus / app->parser.step_crochet;

    GFX::RECT<int> img = {
        161 + (14*type),
        18,
        13,
        clipheight
    };
    Pos sustain;
    if (app->parser.gamenotes[note].flag & FLAG_NOTE_ISOPPONENT) 
        sustain = notePos.opponent[type]; 
    else
        sustain = notePos.player[type];

    int xpos = sustain.x + img.w;
    float ypos = y + (img.h*2);

    for (int i = 0; i < static_cast<int>((length*3.6f) * (app->parser.chartdata.speed)); i++)
    {
        if (i == static_cast<int>((length*3.6f) * (app->parser.chartdata.speed))-1) //draw sustain ends
        {
            img.y = 1;
            img.h = 16;
        }

        GFX::RECT<float> disp = {static_cast<float>(xpos - hudcam.camx.getValue()), ypos + (i*clipheight) - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)};
        if (app->parser.gamenotes[note].flag & FLAG_NOTE_ISOPPONENT && disp.y < sustain.y+img.h*2) {
          //  opponent->setAnim(app->parser.gamenotes[note].type+1);
            continue; //stop drawing opponents note if they were "hit"
        }
        GFX::drawTexZoom<float>(hud, &img, &disp, false, 0, 200, hudcam.zoom.getValue());
    }
}

void PlayStateScreen::drawNotes(void) 
{
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes.size()); i++)
    {
        if (app->parser.gamenotes[i].flag & FLAG_NOTE_HIT)
            continue; //dont draw notes if they are hit
        int type = app->parser.gamenotes[i].type;
        int curNotex;
        float curNotey;

        Pos note;
        if (app->parser.gamenotes[i].flag & FLAG_NOTE_ISOPPONENT)
            note = notePos.opponent[type];
        else
            note = notePos.player[type];

        curNotex = note.x;
        curNotey = ((app->parser.gamenotes[i].pos - app->parser.songTime) * app->parser.chartdata.speed / 3.6f) + note.y;

        GFX::RECT<int> img = {
            1 + (40*type),
            121,
            39,
            39
        };
    
        GFX::RECT<float> disp = {
            static_cast<float>(curNotex - hudcam.camx.getValue()),
            curNotey - hudcam.camy.getValue(),
            static_cast<float>(img.w),
            static_cast<float>(img.h)
        };

        if (app->parser.gamenotes[i].sus != 0) //check if the note is a sustain
            drawSustain(i, curNotey, type);
        if (app->parser.gamenotes[i].flag & FLAG_NOTE_ISOPPONENT && disp.y < note.y)
        {
          //  opponent->setAnim(app->parser.gamenotes[i].type+1);
            continue; //stop drawing opponents note if they were "hit"
        }
        GFX::drawTexZoom<float>(hud, &img, &disp, false, 0, 200, hudcam.zoom.getValue());
    }
}
