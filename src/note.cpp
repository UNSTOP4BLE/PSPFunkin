#include "playstate.h"
#include "main.h"
#include "screen.h"
#include "chartparser.h"
#include "psp/font.h"

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

void PlayStateScreen::deleteNote(int note, bool opponent) {
    PrintFont(Left, 0, 60, "deleting note");
    if (startnote[opponent] < note)
        startnote[opponent] = note;
}

void PlayStateScreen::drawSustain(int note, float y, int type) 
{
    /*
    int clipheight = 11;
    double length = app->parser.gamenotes[note].sus / app->parser.step_crochet;
    length = (length*3.6f) * (app->parser.chartdata.speed);

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

    for (int i = 0; i < static_cast<int>(length); i++)
    {
        if (i == static_cast<int>(length)-1) //draw sustain ends
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
    if (y+length < 0-length*clipheight)
    {
        PrintFont(Left, 0, 60, "deleting note (sustain)");
        deleteNote(note);
    }
/*
    for (int i = 0; i < static_cast<int>(length); i++)
    {
        if (i == static_cast<int>(length)-1) //draw sustain ends
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
    }*/
}

void PlayStateScreen::drawNotes(bool isopponent) 
{
    for (int i = startnote[isopponent]; i < (isopponent ? static_cast<int>(app->parser.gamenotesopponent.size()) : static_cast<int>(app->parser.gamenotesplayer.size())); i++)
    {
        std::vector<Note> &notes = (isopponent ? app->parser.gamenotesopponent : app->parser.gamenotesplayer);


        if (notes[i].flag & FLAG_NOTE_HIT)
        {
            if (notes[i].sus == 0)
                deleteNote(i, isopponent);
            continue; //dont draw notes if they are hit
            i = startnote[isopponent];
        }
        int type = notes[i].type;
        int curNotex;
        float curNotey;

        Pos note;
        if (isopponent)
            note = notePos.opponent[type];
        else
            note = notePos.player[type];

        curNotex = note.x;
        curNotey = ((notes[i].pos - app->parser.songTime) * app->parser.chartdata.speed/3.7) + note.y;

        //delete note if offscreen
        if (curNotey < 0 && notes[i].sus == 0)
        {
            deleteNote(i, isopponent);
            i = startnote[isopponent];
        }
        //note is below the screen, so go back to index 0
        if (curNotey > GFX::SCREEN_HEIGHT)
        {
            continue;
            i = startnote[isopponent];
        }

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

        if (notes[i].sus != 0) //check if the note is a sustain
            drawSustain(i, curNotey, type);
        GFX::drawTexZoom<float>(hud, &img, &disp, false, 0, 200, hudcam.zoom.getValue());
    }
}
//        if (app->parser.gamenotes[i].flag & FLAG_NOTE_ISOPPONENT && disp.y < note.y)
  //      {
    //      //  opponent->setAnim(app->parser.gamenotes[i].type+1);
      //      continue; //stop drawing opponents note if they were "hit"
        //}
