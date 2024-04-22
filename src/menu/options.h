#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class OptionsScreen : public Screen {
public:
    OptionsScreen(int songpos);
    void update(void);
    void draw(void);
    ~OptionsScreen(void); 
private:
    GFX::Texture *background;
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
};