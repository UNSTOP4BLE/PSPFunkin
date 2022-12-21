#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <chrono>
#include "psp/callbacks.h"
#include "psp/pad.h"
#include "psp/font.h"
#include "psp/audio.h"
#include "psp/glib2d.h"
#include "game.h"
#include "error.h"

#include "characters/dad.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

int main()
{
    setupcallbacks();
    Pad_Init();
    Audio_Init();
    g2dInit();
    FntInit("assets/font/font.png");

    Mix_Music *test = Audio_LoadSong("assets/Vocals.wav");
   	Audio_PlaySong(test, false);

    Dad_Init();

 //  	Dad_SetAnim(ANIM_IDLE);
    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();
        g2dClear(GREEN);
        Pad_Update();  

        if (Pad_Pressed(PSP_CTRL_UP))
        	Dad_SetAnim(ANIM_UP);
        Dad_Tick();

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