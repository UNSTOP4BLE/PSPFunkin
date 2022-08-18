#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include "psp/callbacks.h"
#include "psp/pad.h"
#include "psp/font.h"
#include "psp/wav.h"
#include "psp/glib2d.h"
#include "game.h"
#include "error.h"


#include <psprtc.h>
#include "chartparser.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);
 
int main()
{
    setupcallbacks();
    Pad_Init();
    Wav_Init();
    g2dInit();
    FntInit("assets/font/font.png");

    loadChart("assets/chart/thorns-hard.json");
    readInitialData();
    Section new_section = readChartData(0, 0);

    Wav *bopeebo = Wav_Load("assets/Vocals.wav");
    Wav_Play(bopeebo);
    while(1)
    {
        g2dClear(GREEN);
        Pad_Update();
        
        parser.songPos += 1;
        parser.curStep = (parser.songPos/ parser.step_crochet) * 30;

 //          readChart(&new_section);
        PrintMSG(0, 0, "%d", parser.curStep);


        if ((parser.curStep % 32) == 31) 
        	PrintMSG(0, 10, "PEACE");
        switch (game.gamestate)
        {
            case 4: //error
                ErrMSG();
            break;
        }

        g2dFlip(G2D_VSYNC);
    }

    Pad_Shutdown();
    FntFree();

    return 0;
}
