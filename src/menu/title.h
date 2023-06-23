#pragma once

#include "../screen.h"
#include <string>  
#include "../psp/animation.h"
#include "../psp/audio_streamed_file.h"
class TitleScreen : public Screen {
public:
    TitleScreen(void);
    void update(void);
    void draw(void);
    ~TitleScreen(void);
private:
    GFX::Texture *ng;
    Anim_OBJECT titleGF;
    std::string funnymessage[2];
    Audio::StreamedFile *freaky; 
    Audio::AudioBuffer *confirm;
    int curStep;
};
