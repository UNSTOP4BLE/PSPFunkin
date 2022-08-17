#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <time.h>
#include "psp/callbacks.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

int main() 
{
    setupcallbacks();
	Wav_Init();
	Pad_Init();
  	g2dInit();
  	FntInit("assets/font.png");

  	while(1)
  	{
       	g2dClear(G2D_RGBA(84, 192, 201, 0));

       	Pad_Update();

		g2dFlip(G2D_VSYNC);
  	}

  	Pad_Shutdown();
  	FntFree();

  	return 0;
}