#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include "psp/callbacks.h"
#include "psp/pad.h"
#include "psp/font.h"
#include "psp/wav.h"
#include "psp/glib2d.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

int main() 
{
    setupcallbacks();
	Pad_Init();
  	g2dInit();
  	FntInit("assets/font.png");

  	while(1)
  	{
       	g2dClear(G2D_RGBA(84, 192, 201, 0));
       	Pad_Update();

  //     	PrintMSG(0, 0, "asd");

		g2dFlip(G2D_VSYNC);
  	}

  	Pad_Shutdown();
  	//FntFree();

  	return 0;
}