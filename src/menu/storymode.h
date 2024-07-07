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
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
    std::vector<std::string> songs;
    Audio::AudioBuffer *option;
    Audio::AudioBuffer *back;
    Audio::AudioBuffer *confirm;
    Tween<float, QuadInOutEasing, Chrono> backgroundy;
};