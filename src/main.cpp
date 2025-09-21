#define SDL_MAIN_HANDLED

#include "app.hpp"
#include <SDL2/SDL.h>
#include <chrono>
#include <cassert>

#ifdef PSP
#include "engine/psp/rendererpsp.hpp"
#include "engine/psp/callbacks.h"
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_MODULE_INFO("PSPFunkin", 0, 1, 0);
#else
#include "engine/pc/renderersdl.hpp"
#endif

PSPFunkin g_app;

#include "scenes/title.hpp"
#include "scenes/playstate.hpp"

int main()
{
    //get a random number seed
    srand(time(NULL));

    int e; //error code for sdl
    uint32_t sdlflags = 0;
    sdlflags |= SDL_INIT_AUDIO;

    //todo this is plain wrong lmao, on other platfroms sdl_init never gets executed
#ifdef PSP
    setupcallbacks();
    g_app.renderer = new GFX::PSPRenderer();
    INPUT::ControllerDevice inputDevice;
#else
    sdlflags |= SDL_INIT_VIDEO;
    sdlflags |= SDL_INIT_EVENTS;
    e = (SDL_Init(sdlflags) >= 0);//SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO) >= 0);
    g_app.renderer = new GFX::SDLRenderer();
    
    assert(e);
    INPUT::KeyboardDevice inputDevice;
#endif
    g_app.renderer->init();
    g_app.trans.init();

    g_app.audiomixer.start();


   // setScreenCol(0xFF00FF00);
    
    SCENE::set(new TitleSCN());

    g_app.timer.start();
    auto last_t = std::chrono::high_resolution_clock::now();
    while (g_app.renderer->running())
    {
        g_app.renderer->beginFrame();
#ifndef PSP
        if (!g_app.trans.isrunning())
            SDL_PumpEvents();
#endif
        inputDevice.getEvent(g_app.input);

        g_app.curscene->update();  
        g_app.trans.update(g_app.timer.elapsedMS()/1000);
        g_app.curscene->draw();  
        g_app.trans.draw();

        g_app.renderer->endFrame();
        auto cur_t = std::chrono::high_resolution_clock::now();
        g_app.deltatime = std::chrono::duration<double, std::milli>(cur_t - last_t).count();

        last_t = cur_t;
    }


    return 0;
}