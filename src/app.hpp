#pragma once 
#include "engine/audio_mixer.hpp"
#include "engine/scene.hpp"
#include "engine/renderer.hpp"
#include "engine/file.hpp"
#include "engine/timer.hpp"
#include "engine/input.hpp"
#include "trans.hpp"

class PSPFunkin {
public:
    GFX::Renderer *renderer; 
    INPUT::Event input;
    GAME::Transition trans;
    TIMER::Timer timer;
    float deltatime; //todo put this in timer
    Audio::Mixer audiomixer; //todo this bothers me lol capitalize audio napespace
    SCENE::Scene *curscene;
};

extern PSPFunkin g_app;
