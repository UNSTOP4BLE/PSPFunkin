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
        Gfx::RectWH<int> img = {1 + (40*i), 1, 39, 39};
        
        //opponent
        Gfx::RectWH<int> disp = {notePos.opponent[i].x - hudcam.camx.getValue(), notePos.opponent[i].y - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)};  
        //GFX::drawTex<float>(&hud->image, &img, &disp, 0, 200, hudcam.zoom.getValue());

        //player
        if (checkPadHeld[i] && notehit[i]) //hit
        {
            int maxframe = 3-1;
            if (checkPad[i])
                noteframe[i].setValue(0, static_cast<float>(maxframe), static_cast<float>(maxframe)/static_cast<float>(24));
            if (static_cast<int>(noteframe[i].getValue()) > maxframe)
                noteframe[i].setValue(maxframe);
        
            img.y = 1 + (40*static_cast<int>(noteframe[i].getValue()));
            img.y += 40*4;
        }
        else if (checkPadHeld[i] && !notehit[i]) //not hit
        {
            int maxframe = 2-1;
            if (checkPad[i])
                noteframe[i].setValue(0, static_cast<float>(maxframe), static_cast<float>(maxframe)/static_cast<float>(24));
    
            if (static_cast<int>(noteframe[i].getValue()) > maxframe)
                noteframe[i].setValue(maxframe);

            img.y = 1 + (40*static_cast<int>(noteframe[i].getValue()));            
            img.y += 40*1;
        }

        disp = {notePos.player[i].x - hudcam.camx.getValue(), notePos.player[i].y - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)}; 
        //GFX::drawTex<float>(&hud->image, &img, &disp, 0, 200, hudcam.zoom.getValue());
    }
}

void PlayStateScreen::deleteNote(int &note, bool opponent) {
    app->parser.gamenotes[opponent].erase(app->parser.gamenotes[opponent].begin() + note);
    note = -1;
}

void PlayStateScreen::drawSustain(int note, float y, int type, bool isopponent) 
{
    int length = ((app->parser.gamenotes[isopponent][note].sus * app->parser.chartdata.speed) / SUSTAIN_CLIPHEIGHT) - 1; //maybe right

    Gfx::RectWH<int> img = {
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

        Gfx::RectWH<int> disp = {xpos - hudcam.camx.getValue(), ypos + (i*SUSTAIN_CLIPHEIGHT) - hudcam.camy.getValue(), img.w, img.h};
  
        if ((disp.y+SUSTAIN_CLIPHEIGHT < sustain.y && app->parser.gamenotes[isopponent][note].flag & FLAG_NOTE_HIT) || (disp.y+SUSTAIN_CLIPHEIGHT < 0)) {
            continue;
        }
        if ((isopponent || (!isopponent && notehit[type])) && disp.y < sustain.y+SUSTAIN_CLIPHEIGHT) {
            continue; //stop drawing note if they were hit
        }

        //GFX::drawTex<float>(&hud->image, &img, &disp, 0, 200, hudcam.zoom.getValue());
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
        if (curNotey > app->renderer->height)
            break;
        if (curNotey+(notes[i].sus * app->parser.chartdata.speed) < 0 && notes[i].sus != 0) {
            deleteNote(i, isopponent);
            continue;
        }

        if (notes[i].sus != 0) //check if the note is a sustain
            drawSustain(i, curNotey, type, isopponent);
        if (!(notes[i].flag & FLAG_NOTE_HIT))
        {
            Gfx::RectWH<int> img = {
                1 + (40*type),
                121,
                39,
                39
            };
        
            Gfx::RectWH<int> disp = {
                curNotex - hudcam.camx.getValue(),
                curNotey - hudcam.camy.getValue(),
                img.w,
                img.h
            };

            //GFX::drawTex<float>(&hud->image, &img, &disp, 0, 200, hudcam.zoom.getValue());
        }
    }
}