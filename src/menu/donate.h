#pragma once

#include "../screen.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

class DonateScreen : public Screen {
public:
    DonateScreen(void);
    void update(void);
    void draw(void);
    ~DonateScreen(void); 
private:
    GFX::Texture *background;
};