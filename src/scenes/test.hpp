#pragma once

#include "../app.hpp"

#include "../engine/animation.hpp"

class TestSCN : public SCENE::Scene {
public:
    TestSCN(void);
    void update(void);
    void draw(void);
    ~TestSCN(void); 
private:
    ANIMATION::Animatable obj;
};