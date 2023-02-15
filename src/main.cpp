#define SDL_MAIN_HANDLED
#include "main.h"
#include "screen.h"
#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <chrono>
#include "psp/callbacks.h"
#include "psp/font.h"
#include "psp/pad.h"

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

double deltaTime;

int main()
{
    //get a random number seed
    srand(time(NULL));

    setupcallbacks();
    Pad_Init();
    Audio_Init();
    g2dInit();
    FntInit();
    setScreenCol(GREEN);
    
    setScreen(new TitleScreen());

    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();

        g2dClear(screenCol);
        Pad_Update();  

        if (currentScreen == NULL)
            ErrMSG("SCREEN IS NULL");           
    
        currentScreen->update();  
        currentScreen->draw();  

        g2dFlip(G2D_VSYNC);

        auto current = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
        last = current;
    }

    return 0;
}

double getDT(void)
{
    return deltaTime;
}
