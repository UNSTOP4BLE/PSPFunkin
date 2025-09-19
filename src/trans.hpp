#pragma once 

#include "engine/renderer.hpp"
#include "engine/tween.hpp"

namespace GAME {
constexpr int TRANS_PIXELS = 64;
constexpr float TRANS_SPEED = 2; //seconds 
constexpr int TRANS_EXTRA_BUF = 16; //pixels 

class Transition {
public:
    inline void init(void (*loadFunc)(void) = nullptr) {loadedassets = false; 
                                                        y.setValue(-(GFX::SCREEN_HEIGHT+TRANS_EXTRA_BUF+TRANS_PIXELS*2)); 
                                                        dst = {0, static_cast<int>(y.getValue()), GFX::SCREEN_WIDTH, GFX::SCREEN_HEIGHT+TRANS_EXTRA_BUF};
                                                        loadFunction = loadFunc; 
                                                       };
    inline void start(void) {y.setValue(GFX::SCREEN_HEIGHT+TRANS_PIXELS, TRANS_SPEED);};
    void update(float time);
    void draw(void);
private:
    GFX::RECT<int32_t> dst;
    Tween<float, LinearEasing> y;
    bool loadedassets;
    void (*loadFunction)(void);
};

}