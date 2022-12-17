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

#include "characters/dad.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

int main()
{
    setupcallbacks();
    Pad_Init();
    Wav_Init();
    g2dInit();
    FntInit("assets/font/font.png");

    Dad_Init();

    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();
        g2dClear(GREEN);
        Pad_Update();  

   		Dad_SetAnim(ANIM_IDLE);
        //Dad_Tick();

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