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

#include "psp/animation.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

int main()
{
    setupcallbacks();
    Pad_Init();
    Wav_Init();
    g2dInit();
    FntInit("assets/font/font.png");

	Anim_OBJECT dad;

	g2dTexture *tex[2];
	tex[0] = g2dTexLoad("assets/characters/dad/sheet0.png", G2D_SWIZZLE);
	tex[1] = g2dTexLoad("assets/characters/dad/sheet1.png", G2D_SWIZZLE);
	
	AnimFrames left[] =
	{
		{tex[0], 0, 0, 118, 200, 0, 0},
		{tex[0], 118, 0, 114, 200, 0, 0}
	};
	int leftconf[] = {0, 1};


    AnimOBJECT_SetAnim(&dad, &left[0], &leftconf[0], 20);

    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();
        g2dClear(GREEN);
        Pad_Update();  

        if (Pad_Pressed(PSP_CTRL_LEFT))
   			AnimOBJECT_SetAnim(&dad, &left[0], &leftconf[0], 20);

        AnimOBJECT_Tick(&dad);
        AnimOBJECT_Draw(&dad, 20, 20);

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