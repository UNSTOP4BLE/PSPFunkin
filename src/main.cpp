#define SDL_MAIN_HANDLED

#include <cstdio>
#include "main.h"
#include "app.h"
#include "screen.h"
#ifdef PSP
#include "psp/gfx_renderer_psp.h"
#include <pspdebug.h>
#include <pspkernel.h>
#include <psputility.h>
#include "psp/callbacks.h"
#endif
#include <SDL2/SDL.h>
#include <chrono>
#include "psp/font.h"
#include "psp/font.h"
#include "psp/input.h"

#include "menu/title.h"
#ifdef _WIN32
#include "psp/gfx_renderer_opengl.h"
#include <windows.h>
#include <psapi.h>
#endif 
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
        app->renderer->clear(app->screenCol, 0);
        inputDevice.getEvent(app->event);
        app->normalFont->Print(Left, 0, 0, errstr);
        debugmenu(debugy);
        
        app->renderer->swapBuffers();
    }
}

void debugLog(const char *format, ...)
{
    va_list list;
    char string[1024] = "";
    va_start(list, format);
    std::vsprintf(string, format, list);
    va_end(list);

    printf(string);
    printf("\n");
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
#ifdef PSP
    app->renderer = new Gfx::PSPRenderer(480, 272);
#else
    app->renderer = new Gfx::OpenGLRenderer(1280, 720);
#endif
    if (!app->renderer)
    {
        char buffer[512];
        snprintf(buffer, sizeof(buffer), "renderer initialization failed");

#ifdef _WIN32
        MessageBox(nullptr, buffer, "PSPFunkin error", MB_ICONERROR | MB_OK);
#else
        debugLog(buffer); 
#endif
        exit(1);
    }
    
    app->draw_ctx.renderer = app->renderer;
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
        
        app->draw_ctx.beginFrame();
#ifndef PSP
        SDL_PumpEvents();

/*
        if (Input::windowClosed()) {
            SDL_DestroyWindow(app->window);
            app->window = NULL;
            app->screenSurface = NULL;
            SDL_Quit();
            abort();
        }*/
#endif
        inputDevice.getEvent(app->event);
        ASSERTFUNC(app->currentScreen, "screen is NULL");                
        //app->currentScreen->update();  
      //  app->currentScreen->draw();  

        Gfx::Line line = {
            {0xffff0000, 10,20, 1},
            {0xffff0000, 20,40, 1}
        };
//        app->draw_ctx.drawLine(line);
    
        Gfx::Triangle triangle = {
            {0xffff0000, 10,20, 1},
            {0xffff0000, 20,40, 1}, 
            {0xffff0000, 40,50, 1}
        };
//        app->draw_ctx.drawTriangle(triangle);
    
#ifdef _WIN32 
        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
        app->normalFont->Print(Left, 0, 0, "FPS: %d RAM: %f", static_cast<int>(fps), virtualMemUsedByMe/0x100000);
#else
        app->normalFont->Print(Left, 0, 0, "FPS: %d", static_cast<int>(fps));
#endif

        if (app->event.isPressed(Input::DEBUG_SHOW)) 
            debugshown = !debugshown; 
        if (debugshown)
            debugmenu(debugy);

        app->draw_ctx.endFrame();

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