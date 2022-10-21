#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <chrono>
#include "psp/callbacks.h"
#include "psp/pad.h"
#include "psp/font.h"
#include "psp/wav.h"
#include "psp/glib2d.h"
#include "game.h"
#include "error.h"

#include "chartparser.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);
 
int notePos[2][8] = {
	//x
	{27,  67,  107, 147, //opponent	
     294, 334, 374, 414}, //player
	//y
	{14,  14,  14,  14, //opponent	
     14,  14,  14,  14}, //player
};

void DrawDummyNotes(g2dTexture *note)
{
	Rect img = {0, 1, 39, 39};
	Rect disp = {0, 0, 39, 39};

	for (int i = 0; i < 8; i++)
	{
		img.x += 1;
		if (i != 0)
			img.x += 39;
		if (i == 4) //loop the image rect back for player notes
			img.x = 1;
		disp.x = notePos[0][i];
		disp.y = notePos[1][i];
		DrawG2DTex(note, &img, &disp, true, 0, 200);
	}
}

void DrawNote(g2dTexture *note, double pos, int type, double sus, bool musthit)
{
	Rect notes_img[4] = {  
		{1, 1, 39, 39}, //left
		{41, 1, 39, 39}, //down
		{81, 1, 39, 39}, //up
		{121, 1, 39, 39} //right
	};

	Rect disp = {0, 0, 39, 39};

	if (musthit)
		disp.x = notePos[0][4 + type];
	else
		disp.x = notePos[0][type];

	//disp.y = ((pos - parser.songPos) * parser.initspeed * 0.25);
	disp.y = ((pos - parser.songPos) * parser.initspeed * 0.25);
	DrawG2DTex(note, &notes_img[type], &disp, true, 0, 200);
}

int main()
{
    setupcallbacks();
    Pad_Init();
    Wav_Init();
    g2dInit();
    FntInit("assets/font/font.png");








    loadChart("assets/chart/bopeebo.json");
    readInitialData();
    Section section;
	section.lengthInSteps = 16;


	//parser.noteScroll = -5000;
    Wav *bopeebo = Wav_Load("assets/Vocals.wav");
    Wav_Play(bopeebo);

	g2dTexture* notetex = g2dTexLoad("assets/hud.png", G2D_SWIZZLE);
    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();
        g2dClear(GREEN);
        Pad_Update();  




        tickStep();

        section = readChartData(parser.curStep / section.lengthInSteps);

        PrintMSG(0, 0, "%d %d %f %fsection%d musthit %d", parser.curStep, parser.songPos, parser.step_crochet, game.deltaTime, parser.curStep/16, section.mustHitSection);

        DrawNote(notetex, section.sectionNotes[0], section.sectionNotes[1], section.sectionNotes[2], section.mustHitSection);
        DrawDummyNotes(notetex);

        if ((parser.curStep % 32) == 31) 
        	PrintMSG(0, 10, "PEACE");














        switch (game.gamestate)
        {
            case 4: //error
                ErrMSG();
            break;
        }

        g2dFlip(G2D_VSYNC);

        auto current = std::chrono::high_resolution_clock::now();
		game.deltaTime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
		last = current;
    }

    Pad_Shutdown();
    FntFree();

    return 0;
}
