#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  
#include "../psp/assetmanager.h"   

class FreeplayScreen : public Screen {
public:
    FreeplayScreen(int songpos);
    void update(void);
    void draw(void);
    ~FreeplayScreen(void); 
private:
    const ImageAsset *background;
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
    std::vector<std::string> songs;
    const SoundAsset *option;
    const SoundAsset *back;
    const SoundAsset *confirm;
    Tween<float, QuadInOutEasing, Chrono> textx;
    Tween<float, QuadInOutEasing, Chrono> texty;
};