#define SDL_MAIN_HANDLED
#include "main.h"
#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <chrono>
#include "psp/callbacks.h"
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

    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();

        g2dClear(GREEN);
        Pad_Update();  

        if (currentScreen == NULL)
			ErrMSG("SCREEN IS NULL");			
	
		currentScreen->update();  

        g2dFlip(G2D_VSYNC);

        auto current = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
		last = current;
    }

    return 0;
}

double getDT()
{
	return deltaTime;
}