#pragma once

#include "../screen.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class StoryModeScreen : public Screen {
public:
    StoryModeScreen(void);
    void update(void);
    void draw(void);
    ~StoryModeScreen(void); 
private:
    GFX::Texture *background;
    //selections
    int selection;
    std::vector<std::string> songs;
    Tween<float, QuadInOutEasing, Chrono> backgroundy;
};