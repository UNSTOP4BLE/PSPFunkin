#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class StoryModeScreen : public Screen {
public:
    StoryModeScreen(int songpos);
    void update(void);
    void draw(void);
    ~StoryModeScreen(void); 
private:
    GFX::Texture *background;
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
    std::vector<std::string> songs;
    Tween<float, QuadInOutEasing, Chrono> backgroundy;
};