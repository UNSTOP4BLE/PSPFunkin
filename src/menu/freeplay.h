#pragma once

#include "../screen.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class FreeplayScreen : public Screen {
public:
    FreeplayScreen(void);
    void update(void);
    void draw(void);
    ~FreeplayScreen(void); 
private:
    GFX::Texture *background;
    //selections
    int selection;
    std::vector<std::string> songs;
    Tween<float, QuadInOutEasing, Chrono> textx;
    Tween<float, QuadInOutEasing, Chrono> texty;
};