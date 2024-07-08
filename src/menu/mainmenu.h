#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include "../psp/animation.h"
#include <string>            
#include <vector>  
#include "../psp/assetmanager.h"   

class MainMenuScreen : public Screen {
public:
    MainMenuScreen(int songpos);
    void update(void);
    void draw(void);
    ~MainMenuScreen(void); 
private:
    const ImageAsset *background;
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
    Anim_OBJECT *menu_selections[4];
    Tween<float, QuadInOutEasing, Chrono> backgroundy;
    const SoundAsset *option;
    const SoundAsset *back;
    const SoundAsset *confirm;
};