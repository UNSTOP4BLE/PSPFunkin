#pragma once

#include "../app.hpp"
#include "../engine/object.hpp"

class TitleSCN : public SCENE::Scene {
public:
    TitleSCN(void);
    void update(void);
    void draw(void);
    ~TitleSCN(void); 
private:
    OBJECT::ObjectGeneric gf;
    OBJECT::ObjectGeneric logo;
    OBJECT::ObjectGeneric pressenter;
    float stepcrochet;
    float curstep;
    std::string funnymessage[2];
};