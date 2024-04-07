#pragma once

#include "../screen.h"
#include "../psp/tween.h"
#include <string>            
#include <vector>  

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
    std::vector<std::string> songs;
    Tween<float, QuadInEasing, Chrono> backgroundy;
};