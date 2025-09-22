#include "object.hpp"
#include "engine/assets.hpp"
#include "engine/renderer.hpp"


namespace OBJECT {

void Object2D::init(ASSETS::AssetManager &mgr, std::string path) {
    
    setScale(1);
    CAMERA::Camera defaultcam(0,0,0);
    setCamera(defaultcam);
}

void Object2D::update(float t) {
}

void Object2D::draw(GFX::Renderer *renderer, GFX::RECT<int32_t> &src, GFX::RECT<int32_t> &dst) {
    renderer->drawTexRect(tex->image, src, dst, cam.z, 0xFFFFFFFF);
}

} //namespace OBJECT