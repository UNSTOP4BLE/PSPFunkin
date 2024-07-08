#pragma once

#include "../screen.h"
#include <string>  
#include "../psp/animation.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/assetmanager.h"   

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
    const ImageAsset *ng;
    Anim_OBJECT *titleGF;
    Anim_OBJECT *logo;
    std::string funnymessage[2];
    Audio::StreamedFile *freaky; 
    const SoundAsset *confirm;
    int curStep;
};
