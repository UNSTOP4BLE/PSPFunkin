#define SDL_MAIN_HANDLED
#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <chrono>
#include "psp/callbacks.h"
#include "game.h"
#include "common.h"

PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

double deltaTime;

int main()
{
    setupcallbacks();
    Pad_Init();
    Audio_Init();
    g2dInit();
    FntInit("assets/font/font.png");
    
    GameScreens gamescr;       
	Screen *currentScreen = &gamescr.Menu_Title; //remove later
	currentScreen->init();
    
    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();

        g2dClear(GREEN);
        Pad_Update();  

	    currentScreen->update();

        g2dFlip(G2D_VSYNC);

        auto current = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
		last = current;
    }

    return 0;
}