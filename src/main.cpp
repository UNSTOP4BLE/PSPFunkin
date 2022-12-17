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

	int leftconf[] = {0, 1};

    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();
        g2dClear(GREEN);
        Pad_Update();  

        if (Pad_Held(PSP_CTRL_LEFT))
   			AnimOBJECT_SetAnim(&dad, &left[0], &leftconf[0], 15, sizeof(leftconf)/sizeof(leftconf[0]));

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