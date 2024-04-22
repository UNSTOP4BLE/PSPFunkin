#pragma once

#include "../screen.h"
#include "../psp/audio_streamed_file.h"
#include "../psp/tween.h"
#include "../psp/animation.h"
#include <string>            
#include <vector>  

class MainMenuScreen : public Screen {
public:
    MainMenuScreen(int songpos);
    void update(void);
    void draw(void);
    ~MainMenuScreen(void); 
private:
    GFX::Texture *background;
    Audio::StreamedFile *freaky; 
    //selections
    int selection;
    Anim_OBJECT *menu_selections[4];
    Tween<float, QuadInOutEasing, Chrono> backgroundy;
};