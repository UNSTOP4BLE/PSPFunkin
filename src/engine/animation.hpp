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

    void calcDuration_FPS(void) {
        duration = getIndicieCount()/getFPS();
    }

    void calcDuration_Step(float stepspersec) {
        float fps = getFPS();
        float maxduration = fps * stepspersec;
        float minduration = getIndicieCount() / fps;
        duration = std::min(maxduration, minduration);
    }
    void playAnim(void);
    void playAnim(uint32_t anim_h) {setAnim(anim_h); playAnim();}
    KeyFrame *getCurFrame(void) {
        return curkeyframe; 
    }
    const ASSETS::ImageAsset *getCurTex(void) {
        return curtex->tex;
    }

    void setLoop(bool l) {loop = l;}
    bool isPlaying(void) {return curframe.isDone();}

private:
    bool loop;
    const ASSETS::JsonAsset *config;
    float duration; //animation must end

    Animation *curanim;
    Tween<float, LinearEasing> curframe;

    std::vector<KeyFrame> keyframes;
    std::vector<AnimTex> textures;
    std::vector<Animation> animations;
    KeyFrame *curkeyframe;
    AnimTex *curtex;

    //helpers
    int findAnimationIndex(uint32_t hash);
    AnimTex* findTexture(uint32_t hash);
    int getIndicieCount(void) {return curanim->indices.size();}
    int getFPS(void) {return curanim->fps;}
};

} //namespace ANIMATION