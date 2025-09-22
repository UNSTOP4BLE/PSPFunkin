#pragma once 

#include "assets.hpp"
#include "tween.hpp"
#include <iomanip>

namespace ANIMATION {

struct KeyFrame {
    uint32_t tex_h;
    GFX::RECT<int32_t> src;
    int ox, oy;
};

struct AnimTex {
    uint32_t tex_h;
    const ASSETS::ImageAsset *tex;
};

struct Animation {
    uint32_t anim_h;
    int fps;
    std::vector<int> indices;
};

class Animatable {
public:
    void init(ASSETS::AssetManager &mgr, std::string path);
    void update(float t);

    void setAnim(uint32_t anim_h);
    void setDuration(float f);
    void playAnim(void);
    void playAnim(uint32_t anim_h) {setAnim(anim_h); playAnim();}
    bool isPlaying(void) {return curframe.isDone();}
    KeyFrame *curkeyframe;
    AnimTex *curtex;
    int getIndicieCount(void) {return curanim->indices.size();}
    int getFPS(void) {return curanim->fps;}
    void setloop(bool l) {loop = l;}

private:
    bool loop;
    const ASSETS::JsonAsset *config;
    float duration; //animation must end

    Animation *curanim;
    Tween<float, LinearEasing> curframe;

    std::vector<KeyFrame> keyframes;
    std::vector<AnimTex> textures;
    std::vector<Animation> animations;

    //helpers
    int findAnimationIndex(uint32_t hash);
    AnimTex* findTexture(uint32_t hash);
};

//helpers
inline float calcDuration_FPS(float numindices, float fps) {
    return numindices/fps;
}

inline float calcDuration_Step(float numindices, float fps, float steptosec) {
    float maxduration = fps * steptosec;
    float minduration = calcDuration_FPS(numindices, fps);
    return std::min(maxduration, minduration);
}

} //namespace ANIMATION