#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  
#include "../psp/assetmanager.h"   

class OptionsScreen : public Screen {
public:
    OptionsScreen(int songpos);
    void update(void);
    void draw(void);
    ~OptionsScreen(void); 
private:
    const ImageAsset *background;
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
};