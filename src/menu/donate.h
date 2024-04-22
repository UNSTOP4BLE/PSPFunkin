#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class DonateScreen : public Screen {
public:
    DonateScreen(int songpos);
    void update(void);
    void draw(void);
    ~DonateScreen(void); 
private:
    GFX::Texture *background;
    Audio::StreamedFile *freaky; 
};