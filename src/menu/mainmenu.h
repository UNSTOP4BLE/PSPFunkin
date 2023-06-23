#pragma once

#include "../screen.h"
#include "../psp/tween.h"

class MainMenuScreen : public Screen {
public:
    MainMenuScreen(void);
    void update(void);
    void draw(void);
    ~MainMenuScreen(void); 
private:
    GFX::Texture *background;
    int selection;
    Tween<float, QuadInEasing, ChronoTimeSource> backgroundy;
};