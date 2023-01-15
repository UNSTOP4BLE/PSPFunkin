#define SDL_MAIN_HANDLED
#include "main.h"
#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <chrono>
#include "psp/callbacks.h"
#include "psp/pad.h"
#include "psp/font.h"
#include "psp/audio.h"
#include "psp/glib2d.h"

#include "playstate.h" //remove later
PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);
int main()
{
    setupcallbacks();
    Pad_Init();
    Audio_Init();
    g2dInit();
    FntInit("assets/font/font.png");
        
    Game game;
	Screen *currentScreen = game.Playstate; //remove later
	Screen->init();
    
    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();
        g2dClear(GREEN);
        Pad_Update();  

	    currentScreen->update();

        g2dFlip(G2D_VSYNC);

        auto current = std::chrono::high_resolution_clock::now();
		currentScreen->deltaTime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
		last = current;
    }

    return 0;
}