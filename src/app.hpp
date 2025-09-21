#pragma once 
#include "engine/renderer.hpp"
#include "engine/input.hpp"
#include "trans.hpp"
#include "engine/audio_mixer.hpp"
#include "engine/timer.hpp"
#include "engine/scene.hpp"
#include "engine/assets.hpp"

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
    ASSETS::AssetManager assets;
};

extern PSPFunkin g_app;
