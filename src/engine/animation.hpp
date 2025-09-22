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

//functions for timing
class TimeFPS {
public:
    static inline float apply(int indcount, float fps) {
        return indcount/fps;
    }
};

class TimeSTEP {
public:

    TimeSTEP(void) : stepsPerSec(0) {}

    float stepsPerSec;
    //in this case fps is how many steps the animation should last
    inline float apply(int indcount, float fps) {
//        assert(stepsPerSec > 0); 
        int maxduration = fps * stepsPerSec; //endime * steps per second
        int minduration = indcount/fps;
        return std::min(maxduration, minduration); //end x steps later
    }
}; 

template<typename T> class Animatable {
public:
    void init(ASSETS::AssetManager &mgr, std::string path);
    void update(float t);

    void setAnim(uint32_t anim_h);
    void playAnim(void);
    void playAnim(uint32_t anim_h) {setAnim(anim_h); playAnim();}
    bool isPlaying(void) {return curframe.isDone();}
    KeyFrame *curkeyframe;
    AnimTex *curtex;
private:
    T timemanager; 
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