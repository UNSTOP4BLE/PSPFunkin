#pragma once 
#include "engine/audio_mixer.hpp"
#include "engine/scene.hpp"
#include "engine/renderer.hpp"
#include "engine/file.hpp"
#include "engine/timer.hpp"

class PSPFunkin {
public:
    GFX::Renderer *renderer; 
    TIMER::Timer timer;
    float deltatime; //todo put this in timer
//    Input::Event event;
    Audio::Mixer audiomixer;
    SCENE::Scene *curscene;
};

extern PSPFunkin g_app;
