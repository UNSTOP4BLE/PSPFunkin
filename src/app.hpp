#pragma once 
#include "engine/renderer.hpp"
#include "engine/input.hpp"
#include "trans.hpp"
#include "engine/audio_mixer.hpp"
#include "engine/timer.hpp"
#include "engine/scene.hpp"
#include "engine/audiomanager.hpp"

class PSPFunkin {
public:
    GFX::Renderer *renderer; 
    INPUT::Event input;
    GAME::Transition trans;
    TIMER::Timer timer;
    float deltatime; //todo put this in timer
    AUDIO::Mixer audiomixer;
    SCENE::Scene *curscene;
    //managers
    MANAGER::AudioManager audiomanager;
};

extern PSPFunkin g_app;
