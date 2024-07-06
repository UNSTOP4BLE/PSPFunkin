#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class FreeplayScreen : public Screen {
public:
    FreeplayScreen(int songpos);
    void update(void);
    void draw(void);
    ~FreeplayScreen(void); 
private:
    GFX::Texture *background;
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
    std::vector<std::string> songs;
    Audio::AudioBuffer *option;
    Audio::AudioBuffer *back;
    Audio::AudioBuffer *confirm;
    Tween<float, QuadInOutEasing, Chrono> textx;
    Tween<float, QuadInOutEasing, Chrono> texty;
};