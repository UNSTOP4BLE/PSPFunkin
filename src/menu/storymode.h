#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include "../psp/animation.h"
#include <string>            
#include <vector>  
#include <json/json.h>

class StoryModeScreen : public Screen {
public:
    StoryModeScreen(int songpos);
    void update(void);
    void draw(void);
    ~StoryModeScreen(void); 
private:
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
    std::vector<std::string> songs;
    const JsonAsset *data;
    Anim_OBJECT *bf;
    Anim_OBJECT *opp;
    Anim_OBJECT *gf;
    const SoundAsset *option;
    const SoundAsset *back;
    const SoundAsset *confirm;
    Tween<float, QuadInOutEasing, Chrono> backgroundy;
};