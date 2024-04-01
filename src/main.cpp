#define SDL_MAIN_HANDLED
#include "main.h"
#include "app.h"
#include "screen.h"
#ifdef PSP
#include <pspdebug.h>
#include <melib.h>
#include <pspkernel.h>
#include <psputility.h>
#include "psp/callbacks.h"
#else
#include <SDL2/SDL_image.h>
#endif
#include <SDL2/SDL.h>
#include <chrono>
#include "psp/audio_file_readers.h"
#include "psp/audio_streamed_file.h"
#include "psp/font.h"
#include "psp/font.h"
#include "psp/pad.h"
#include "psp/tween.h"
#include "psp/memory.h"

#include "menu/title.h"

//#define DEBUG //for profiling
#ifdef DEBUG
#include <pspprof.h>
#endif

#ifdef PSP
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);
#endif

PSPFunkin *app;

//error handler
void ErrMSG(const char *filename, const char *function, int line, const char *expr, const char *msg)
{
    char errstr[256];
    sprintf(errstr, "error \nmessage: %s\nexpression: %s \nfile: %s \nfunction: %s \nline %d", msg, expr, filename, function, line);
    printf(errstr);
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

#ifdef PSP
    setupcallbacks();
#endif
  //  SDL_Init(SDL_INIT_AUDIO);
//#else
    ASSERTFUNC(SDL_Init(SDL_INIT_EVERYTHING) >= 0, "failed to init sdl");
//#endif
    J_Init(false);
    Pad_Init();
    app->audioMixer = new Audio::Mixer();
    app->audioMixer->start();
    GFX::init();
    FntInit();
    setScreenCol(0xFF00FF00);
    
    app->timer.start();
    setScreen(new TitleScreen());
    while(1)
    {
        auto last = std::chrono::high_resolution_clock::now();

        if (Pad_Held(PSP_CTRL_CROSS))
            printf("press\n");
#ifndef PSP
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch (event.type) {
                case SDL_WINDOWEVENT:

                    switch (event.window.event) {

                        case SDL_WINDOWEVENT_CLOSE:   // exit game
                            
                            SDL_DestroyWindow(app->window);
                            app->window = NULL;
                            app->screenSurface = NULL;
                            SDL_Quit();
                            abort();
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
#endif
        
        #ifdef DEBUG
        if (Pad_Pressed(PSP_CTRL_SELECT))
            break;      
        #endif
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
#ifdef DEBUG
    gprof_cleanup();
#endif

    J_Cleanup();
    
    return 0;
}