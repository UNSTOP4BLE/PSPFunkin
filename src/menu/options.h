#pragma once

#include "../screen.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class OptionsScreen : public Screen {
public:
    OptionsScreen(void);
    void update(void);
    void draw(void);
    ~OptionsScreen(void); 
private:
    GFX::Texture *background;
    //selections
    int selection;
};