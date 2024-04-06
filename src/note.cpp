#include "playstate.h"
#include "main.h"
#include "screen.h"
#include "chartparser.h"
#include "psp/font.h"
#include "app.h"

void PlayStateScreen::drawDummyNotes(void) 
{
    for (int i = 0; i < 4; i++)
    {
        GFX::RECT<int> img = {1 + (40*i), 1, 39, 39};
        
        //opponent
        GFX::RECT<float> disp = {notePos.opponent[i].x - hudcam.camx.getValue(), notePos.opponent[i].y - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)};  
        GFX::drawTexZoom<float>(hud, &img, &disp, 0, 200, hudcam.zoom.getValue());

        //player
        if (checkPadHeld[i] && notehit[i]) //hit
        {
            if (checkPad[i])
                noteframe[i].setValue(0, static_cast<float>(3-1), static_cast<float>(3-1)/static_cast<float>(24));

            img.y = 1 + (40*static_cast<int>(noteframe[i].getValue()));
            img.y += 40*4;
        }
        else if (checkPadHeld[i] && !notehit[i]) //not hit
        {
            if (checkPad[i])
                noteframe[i].setValue(0, static_cast<float>(2-1), static_cast<float>(2-1)/static_cast<float>(24));
      
            img.y = 1 + (40*static_cast<int>(noteframe[i].getValue()));
            img.y += 40*1;
        }

        disp = {notePos.player[i].x - hudcam.camx.getValue(), notePos.player[i].y - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)}; 
        GFX::drawTexZoom<float>(hud, &img, &disp, 0, 200, hudcam.zoom.getValue());
    }
}

void PlayStateScreen::deleteNote(int note, bool opponent) {
    app->parser.gamenotes[opponent].erase(app->parser.gamenotes[opponent].begin() + note);
//    PrintFont(Left, 0, 60, (opponent ? "deleting oppnote %d" : "\ndeleting pnote %d"), 0);//[opponent]);
}

void PlayStateScreen::drawSustain(int note, float y, int type, bool isopponent) 
{
    if (y+(app->parser.gamenotes[isopponent][note].sus * app->parser.chartdata.speed) < 0) {
        deleteNote(note, isopponent);
    }

    int length = ((app->parser.gamenotes[isopponent][note].sus * app->parser.chartdata.speed) / SUSTAIN_CLIPHEIGHT) - 1; //maybe right

    GFX::RECT<int> img = {
        161 + (14*type),
        18,
        13,
        SUSTAIN_CLIPHEIGHT
    };
    Pos sustain = (isopponent ? notePos.opponent[type] : notePos.player[type]);;

    int xpos = sustain.x + img.w;
    float ypos = y + (img.h*2);

    for (int i = 0; i < length; i++)
    {
        if (i == length-1) //draw sustain ends
            img.y = 1;

        GFX::RECT<float> disp = {static_cast<float>(xpos - hudcam.camx.getValue()), ypos + (i*SUSTAIN_CLIPHEIGHT) - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)};
  
        if ((isopponent || (!isopponent && notehit[type])) && disp.y < sustain.y+img.h) {
            continue; //stop drawing note if they were hit
        }

        GFX::drawTexZoom<float>(hud, &img, &disp, 0, 200, hudcam.zoom.getValue());
    }
}

void PlayStateScreen::drawNotes(bool isopponent) 
{
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes[isopponent].size()); i++)
    {
        std::vector<Note> &notes = app->parser.gamenotes[isopponent];

        if (notes[i].flag & FLAG_NOTE_HIT && notes[i].sus == 0) {
            deleteNote(i, isopponent);
            continue;
        }

        int type = notes[i].type;
        Pos note = (isopponent ? notePos.opponent[type] : notePos.player[type]);
        int curNotex = note.x;
        int curNotey = static_cast<int>(((notes[i].pos - app->parser.songTime) * app->parser.chartdata.speed) + note.y);
        //note is below the screen, so go back to index 0
        if (curNotey > GFX::SCREEN_HEIGHT)
            break;

        if (notes[i].sus != 0) //check if the note is a sustain
            drawSustain(i, curNotey, type, isopponent);
        if (!(notes[i].flag & FLAG_NOTE_HIT))
        {
            GFX::RECT<int> img = {
                1 + (40*type),
                121,
                39,
                39
            };
        
            GFX::RECT<float> disp = {
                curNotex - hudcam.camx.getValue(),
                curNotey - hudcam.camy.getValue(),
                static_cast<float>(img.w),
                static_cast<float>(img.h)
            };

            GFX::drawTexZoom<float>(hud, &img, &disp, 0, 200, hudcam.zoom.getValue());
        }

    }
}