#pragma once

#include "../screen.h"
#include "../psp/tween.h"
#include <string>            

class MainMenuScreen : public Screen {
public:
    MainMenuScreen(void);
    void update(void);
    void draw(void);
    ~MainMenuScreen(void); 
private:
    GFX::Texture *background;
    int selection;
    std::string selectedsong;
    std::string songs[4];
    Tween<float, QuadInEasing, Chrono> backgroundy;
};