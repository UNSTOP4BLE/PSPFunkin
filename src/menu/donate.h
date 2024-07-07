#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  
#include "../psp/assetmanager.h"   

class DonateScreen : public Screen {
public:
    DonateScreen(int songpos);
    void update(void);
    void draw(void);
    ~DonateScreen(void); 
private:
    const ImageAsset *background;
    Audio::StreamedFile *freaky; 
    Audio::AudioBuffer *back;
};