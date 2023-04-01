#define SDL_MAIN_HANDLED
#include "main.h"
#include "screen.h"
#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include <SDL2/SDL.h>
#include <chrono>
#include "psp/callbacks.h"
#include "psp/audio_file_readers.h"
#include "psp/audio_streamed_file.h"
#include "psp/font.h"
#include "psp/font.h"
#include "psp/pad.h"
#include "psp/tween.h"

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

PSPFunkin *app;

//error handler
void ErrMSG(const char *filename, const char *function, int line, const char *expr, const char *msg)
{
    char errstr[256];
    sprintf(errstr, "error \nmessage %s\nexpression: %s \nfile: %s \nfunction %s \nline %d", msg, expr, filename, function, line);
    while(1)
    {
        g2dClear(app->screenCol);
        PrintFont(Left, 0, 0, errstr);
        g2dFlip(G2D_VSYNC);
    }
}

int main()
{
    //get a random number seed
    srand(time(NULL));

    app = new PSPFunkin(); //new pspfunkin every single time?? no need for a rewrite!

    setupcallbacks();
    Pad_Init();
    SDL_Init(SDL_INIT_AUDIO);
    app->audioMixer = new Audio::Mixer();
    app->audioMixer->start();
    g2dInit();
    FntInit();
    setScreenCol(GREEN);
    
    setScreen(new TitleScreen());

    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();

        g2dClear(app->screenCol);
        Pad_Update();  
        ASSERTFUNC(app->currentScreen, "screen is NULL");                
    
        app->currentScreen->update();  
        app->currentScreen->draw();  
        
        g2dFlip(G2D_VSYNC);

        auto current = std::chrono::high_resolution_clock::now();
        app->deltatime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
        last = current;
    }

    return 0;
}