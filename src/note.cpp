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
        
        //opponent
        GFX::RECT<float> disp = {notePos.opponent[i].x - hudcam.camx.getValue(), notePos.opponent[i].y - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)};  
        GFX::drawTexZoom<float>(hud, &img, &disp, true, 0, 200, hudcam.zoom.getValue());

        //player
        if (checkPadHeld[i] && notehit[i])
        {
            notetimer[i] += 2 + app->deltatime;

            noteframe[i] = notetimer[i]/40;
            if (noteframe[i] > 3-1)
                noteframe[i] = 3-1;
            img.y = 1 + (40*noteframe[i]);

            img.y += 40*4;
        }
        else if (checkPadHeld[i] && !notehit[i])
        {
            notetimer[i] += 2 + app->deltatime;

            noteframe[i] = notetimer[i]/40;
            if (noteframe[i] > 2-1)
                noteframe[i] = 2-1;
            img.y = 1 + (40*noteframe[i]);

            img.y += 40*1;
        }
        else
        {
            noteframe[i] = 0;
            notetimer[i] = 0;
            if (!checkPadHeld[i] && notehit[i]) //reset animation for notes
                notehit[i] = false;
        }


        disp = {notePos.player[i].x - hudcam.camx.getValue(), notePos.player[i].y - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)}; 
        GFX::drawTexZoom<float>(hud, &img, &disp, true, 0, 200, hudcam.zoom.getValue());
    }
}

void PlayStateScreen::deleteNote(int note, bool opponent) {
    app->parser.gamenotes[opponent].erase(app->parser.gamenotes[opponent].begin() + note);
//    PrintFont(Left, 0, 60, (opponent ? "deleting oppnote %d" : "\ndeleting pnote %d"), 0);//[opponent]);
}

void PlayStateScreen::drawSustain(int note, float y, int type, bool isopponent) 
{
    /*
    int clipheight = 11;
//    int length = static_cast<int>(app->parser.gamenotes[isopponent][note].sus / app->parser.step_crochet) * (app->parser.chartdata.speed * clipheight);
    int length = static_cast<int>(app->parser.gamenotes[isopponent][note].sus / app->parser.step_crochet) * app->parser.chartdata.speed;

    length /= clipheight;
//((note_position + data[2] - time) * song_speed) - sustain_end_texture_height
    GFX::RECT<int> img = {
        161 + (14*type),
        18,
        13,
        clipheight
    };
    Pos sustain = (isopponent ? notePos.opponent[type] : notePos.player[type]);;

    int xpos = sustain.x + img.w;
    float ypos = y + (img.h*2);

    for (int i = 0; i < length; i++)
    {
        if (i == length-1) //draw sustain ends
        {
            img.y = 1;
            img.h = 16;
        }

        GFX::RECT<float> disp = {static_cast<float>(xpos - hudcam.camx.getValue()), ypos + (i*clipheight) - hudcam.camy.getValue(), static_cast<float>(img.w), static_cast<float>(img.h)};
        GFX::drawTexZoom<float>(hud, &img, &disp, false, 0, 200, hudcam.zoom.getValue());
    }
    if (y+length < 0-length*clipheight)
    {
        deleteNote(note, isopponent);
    }*/
}

void PlayStateScreen::drawNotes(bool isopponent) 
{
    for (int i = 0; i < static_cast<int>(app->parser.gamenotes[isopponent].size()); i++)
    {
        std::vector<Note> &notes = app->parser.gamenotes[isopponent];

        if (notes[i].flag & FLAG_NOTE_HIT) {
            deleteNote(i, isopponent);
            continue;
        }

        int type = notes[i].type;
        Pos note = (isopponent ? notePos.opponent[type] : notePos.player[type]);
        int curNotex = note.x;
        int curNotey = ((notes[i].pos - app->parser.songTime) * app->parser.chartdata.speed) + note.y;

        //note is below the screen, so go back to index 0
        if (curNotey > GFX::SCREEN_HEIGHT)
            break;

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

        //if (notes[i].sus != 0) //check if the note is a sustain
      //      drawSustain(i, curNotey, type, isopponent);
        GFX::drawTexZoom<float>(hud, &img, &disp, false, 0, 200, hudcam.zoom.getValue());
    }
}