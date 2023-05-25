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
#include "psp/memory.h"

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);

PSPFunkin *app;

//error handler
void ErrMSG(const char *filename, const char *function, int line, const char *expr, const char *msg)
{
    char errstr[256];
    sprintf(errstr, "error \nmessage: %s\nexpression: %s \nfile: %s \nfunction: %s \nline %d", msg, expr, filename, function, line);
    while(1)
    {
        GFX::clear(app->screenCol);
        PrintFont(Left, 0, 0, errstr);
        GFX::flip();
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
    GFX::init();
    FntInit();
    setScreenCol(0xFF00FF00);
    
    setScreen(new TitleScreen());
    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();

        GFX::clear(app->screenCol);
        Pad_Update();  
        ASSERTFUNC(app->currentScreen, "screen is NULL");                
        app->currentScreen->update();  
        app->currentScreen->draw();  
        PrintFont(Right, GFX::SCREEN_WIDTH, 0, "Memory usage %d/%d", Mem::curusage/1024/1024, 32);

        GFX::flip();

        auto current = std::chrono::high_resolution_clock::now();
        app->deltatime = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
        last = current;
    }

    return 0;
}