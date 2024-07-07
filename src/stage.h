#pragma once

#include <string>     
#include <vector>     
#include "psp/gfx.h"
#include "psp/assetmanager.h"

class StageObject {
public:
    std::string def; //definition of the texture used 
    GFX::RECT<int> img;
    GFX::RECT<int> initialdisp;
    GFX::RECT<float> disp;
    float scrollfactor[2];
    float angle;
    int alpha;
};

class StageTex {
public:
    std::string def; //definition of the texture used 
    const ImageAsset *texture; //pointer to the actual texture data
};

class Stage {
public:
    void load(const char *jpath, std::string stage);
    void tick(float cx, float cy);
    void drawObjects(std::vector<StageObject> &objs, float camzoom);
    std::vector<StageObject> fgobjects;
    std::vector<StageObject> mdobjects;
    std::vector<StageObject> bgobjects;
private:
    std::vector<StageTex> textures;
};