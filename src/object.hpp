#pragma once

//#include "assets.hpp"
//#include "renderer.hpp"
//#include "timer.hpp"
//#include "tween.hpp"
//#include <vector>

#include "camera.hpp"
#include "engine/animation.hpp"

namespace OBJECT {
class Object2D {
public:
    void init(ASSETS::AssetManager &mgr, std::string path);
    void update(float t);
    void draw(GFX::Renderer *renderer, GFX::XY<int32_t> &pos);
    void setCamera(CAMERA::Camera &c) {cam = c;}
    void setScale(float s) {scale = s;}
    //todo free
    ANIMATION::Animatable animator;
    float stepsPerSecond;
private:
    float scale;
    CAMERA::Camera cam;
};

} //namespace OBJECT