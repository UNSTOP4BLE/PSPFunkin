#pragma once 

#include "assets.hpp"
#include "tween.hpp"

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
    void playAnim(void);
    void playAnim(uint32_t anim_h) {setAnim(anim_h); playAnim();}
    bool isPlaying(void) {return !(curframe.getValue()==getIndicieCount());}
    KeyFrame *curkeyframe;
    AnimTex *curtex;
private:
    const ASSETS::JsonAsset *config;

    Animation *curanim;
    Tween<float, LinearEasing> curframe;

    std::vector<KeyFrame> keyframes;
    std::vector<AnimTex> textures;
    std::vector<Animation> animations;

    //helpers
    int findAnimationIndex(uint32_t hash);
    AnimTex* findTexture(uint32_t hash);
    int getIndicieCount(void) {return curanim->indices.size();}
};

} //namespace ANIMATION