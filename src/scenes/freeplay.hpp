#pragma once 

#include "../app.hpp"
#include <vector>
#include "../engine/object.hpp"

class FreePlaySCN : public SCENE::Scene {
public:
    FreePlaySCN(void);
    void update(void);
    void draw(void);
    ~FreePlaySCN(void); 
private:
};