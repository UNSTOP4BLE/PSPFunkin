#pragma once

#include "../app.hpp"
#include "../object.hpp"

class TitleSCN : public SCENE::Scene {
public:
    TitleSCN(void);
    void update(void);
    void draw(void);
    ~TitleSCN(void); 
private:
    OBJECT::Object2D gf;
  //  OBJECT::ObjectGeneric logo;
    //OBJECT::ObjectGeneric pressenter;
    float stepcrochet;
    float curstep;
    std::string funnymessage[2];
    const ASSETS::JsonAsset *menudata;
};