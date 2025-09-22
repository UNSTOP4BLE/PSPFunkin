#pragma once

#include "camera.hpp"
#include "engine/assets.hpp"

namespace OBJECT {
class Object2D {
public:
    void init(ASSETS::AssetManager &mgr, std::string path);
    void update(float t);
    void draw(GFX::Renderer *renderer, GFX::RECT<int32_t> &src, GFX::RECT<int32_t> &dst);
    
    void setCamera(CAMERA::Camera &c) {cam = c;}
    void setScale(float s) {scale = s;}
    //todo free
private:
    float scale;
    CAMERA::Camera cam;
    const ASSETS::ImageAsset *tex;
};

} //namespace OBJECT