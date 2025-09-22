#pragma once 

#include "../app.hpp"
#include <vector>
#include "../object.hpp"

struct Option {
//    OBJECT::ObjectGeneric obj;
    std::string name;
    GFX::XY<int32_t> offset;
};

class MainMenuSCN : public SCENE::Scene {
public:
    MainMenuSCN(void);
    void update(void);
    void draw(void);
    ~MainMenuSCN(void); 
private:
    const ASSETS::ImageAsset *menubg;
    std::vector<Option> menuoptions;
    int selection;
    Tween<float, SineInOutEasing> menubgy;
};