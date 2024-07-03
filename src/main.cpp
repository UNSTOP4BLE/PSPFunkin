#include <cstdio>
#define SDL_MAIN_HANDLED
#include "main.h"
#include "app.h"
#include "screen.h"
#ifdef PSP
#include <pspdebug.h>
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
#include "psp/input.h"
#include "psp/tween.h"

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
#define DEBUGLOG_MAX 128
void debugmenu(int &debugy)
{
    if (app->event.isHeld(Input::MENU_DOWN)) 
        debugy ++;
    else if (app->event.isHeld(Input::MENU_UP)) 
        debugy --;
    for (int i = debugy; i < static_cast<int>(app->debugmessages.size()); i++)
    {
        while (static_cast<int>(app->debugmessages.size()) > DEBUGLOG_MAX)
            app->debugmessages.erase(app->debugmessages.begin());
        if (i < 0)
            i = 0;
        char linenumber[32];
        sprintf(linenumber, "%d. ", i);
        std::string toprint = linenumber + app->debugmessages[i];
        app->normalFont->Print(Left, 0, (i*10)-debugy*10, toprint.c_str());
    } 
}

//error handler
void ErrMSG(const char *filename, const char *function, int line, const char *expr, const char *msg)
{
    char errstr[512];
    sprintf(errstr, "error\nmessage: %s\nexpression: %s\nfile: %s\nfunction: %s\nline %d", msg, expr, filename, function, line);
    int debugy = -10;
#if defined(PSP) || defined(__vita__)
    Input::ControllerDevice inputDevice;
#else
    Input::KeyboardDevice inputDevice;
#endif
    while(1)
    {
        GFX::clear(app->screenCol);
        inputDevice.getEvent(app->event);
        app->normalFont->Print(Left, 0, 0, errstr);
        debugmenu(debugy);
        
        GFX::flip();
    }
}

void debugLog(const char *format, ...)
{
    va_list list;
    char string[1024] = "";
    va_start(list, format);
    std::vsprintf(string, format, list);
    va_end(list);

    app->debugmessages.push_back(string);
}

void debugLogClear(void)
{
    app->debugmessages.clear();
}


int main()
{
    //get a random number seed
    srand(time(NULL));

    app = new PSPFunkin(); //new pspfunkin every single time?? no need for a rewrite!

    debugLog("debug_start");
#ifdef PSP
    setupcallbacks();
#endif
    ASSERTFUNC(SDL_Init(SDL_INIT_EVERYTHING) >= 0, "failed to init sdl");
    app->audioMixer = new Audio::Mixer();
    app->audioMixer->start();
    GFX::init();
    app->normalFont = new FontManager(Font_Font, getPath("assets/font/font.png").c_str());
    app->boldFont = new FontManager(Font_Bold, getPath("assets/font/boldfont.png").c_str());
    setScreenCol(0xFF00FF00);

#if defined(PSP) || defined(__vita__)
    Input::ControllerDevice inputDevice;
#else
    Input::KeyboardDevice inputDevice;
#endif
    app->timer.start();
    setScreen(new TitleScreen(0, Intro));

    Timer fpsTimer;
    fpsTimer.start();
    float fps = 0;

    bool debugshown = false;
    int debugy = 0;
    while(1)
    {
        std::chrono::time_point<std::chrono::system_clock> last = std::chrono::high_resolution_clock::now();
        
        GFX::clear(app->screenCol);
#ifndef PSP
        SDL_PumpEvents();

        if (Input::windowClosed()) {
            SDL_DestroyWindow(app->window);
            app->window = NULL;
            app->screenSurface = NULL;
            SDL_Quit();
            abort();
        }
#endif
        inputDevice.getEvent(app->event);
        ASSERTFUNC(app->currentScreen, "screen is NULL");                
        app->currentScreen->update();  
        app->currentScreen->draw();  
        app->normalFont->Print(Left, 0, 0, "FPS: %d", static_cast<int>(fps));

        if (app->event.isPressed(Input::DEBUG_SHOW)) 
            debugshown = !debugshown; 
        if (debugshown)
            debugmenu(debugy);

        GFX::flip();

        std::chrono::time_point<std::chrono::system_clock> current = std::chrono::high_resolution_clock::now();
        app->deltatime = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(current - last).count());
        if (fpsTimer.elapsedMS()/1000 > 0.5){
            fps = 1 / (app->deltatime / 1000);
            fpsTimer.stop();
            fpsTimer.start();
        }
        last = current;
    }
#if defined(DEBUG) && defined(PSP)
    gprof_cleanup();
#endif

    return 0;
}