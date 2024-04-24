#pragma once

#include "../screen.h"
#include <string>  
#include "../psp/animation.h"
#include "../psp/audio_streamed_file.h"

enum TitleStates
{
    Intro,
    Flash,
    Title
};

class TitleScreen : public Screen {
public:
    TitleScreen(int songpos, TitleStates _state);
    void update(void);
    void draw(void);
    ~TitleScreen(void);
private:
    TitleStates state;
    GFX::Texture *ng;
    Anim_OBJECT titleGF;
    std::string funnymessage[2];
    Audio::StreamedFile *freaky; 
    Audio::AudioBuffer *confirm;
    int curStep;
};
