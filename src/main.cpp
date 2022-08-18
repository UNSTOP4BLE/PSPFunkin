#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include "psp/callbacks.h"
#include "psp/pad.h"
#include "psp/font.h"
#include "psp/glib2d.h"
#include "game.h"
#include "error.h"

#include "chartparser.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);
 
int main() 
{
    setupcallbacks();
	Pad_Init();
  	g2dInit();
  	FntInit("assets/font/font.png");

  	loadChart("assets/chart/thorns-hard.json");
  	readInitialData();
  	Section new_section = readChartData(0, 0);
  	while(1)
  	{
       	g2dClear(GREEN);
       	Pad_Update();
		
		parser.songPos += 100;
       	parser.curStep = (parser.songPos/parser.step_crochet) / 12;

 //      	readChart(&new_section);
		PrintMSG(0, 0, "%d", parser.curStep);

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