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
 
double next_scroll;
int main()
{
    setupcallbacks();
    Pad_Init();
    Wav_Init();
    g2dInit();
    FntInit("assets/font/font.png");

    loadChart("assets/chart/bopeebo.json");
    readInitialData();
    Section new_section = readChartData(0, 0);


	parser.noteScroll = -30 * 12;
	parser.songTime = parser.noteScroll / parser.step_crochet;

    Wav *bopeebo = Wav_Load("assets/Vocals.wav");
    Wav_Play(bopeebo);


    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();
        g2dClear(GREEN);
        Pad_Update();  

		parser.songTime += 100;
		next_scroll = (parser.songTime * parser.step_crochet) >> 10;
					parser.noteScroll = next_scroll;

		parser.curStep = (parser.noteScroll >> 10);	
		if (parser.noteScroll < 0)
						parser.curStep -= 11;
					parser.curStep /= 12;
		
 //          readChart(&new_section);
        PrintMSG(0, 0, "%d", parser.curStep );


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
