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
char errstr[256];

//error handler
void ErrMSG(const char *filename, const char *function, int line, const char *expr)
{
    sprintf(errstr, "error \nexpression: %s \nfile: %s \nfunction %s \nline %d", expr, filename, function, line);
    while(1)
    {
        g2dClear(screenCol);
        PrintFont(Left, 0, 0, errstr);
        g2dFlip(G2D_VSYNC);
    }

}

int main()
{
    //get a random number seed
    srand(time(NULL));

    setupcallbacks();
    Pad_Init();
    
    SDL_Init(SDL_INIT_AUDIO);
    Audio::mixer.start(44100, );
    g2dInit();
    FntInit();
    setScreenCol(GREEN);
    
    setScreen(new TitleScreen());

    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();

        g2dClear(screenCol);
        Pad_Update();  
        ASSERTFUNC(currentScreen);                
    
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
