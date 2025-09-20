#pragma once
#include "renderer.hpp"
#include "timer.hpp"
#include "tween.hpp"
#include <vector>

namespace OBJECT {

struct KeyFrame {
    uint32_t tex_hash;
    int x, y, w, h, ox, oy;
};

struct AnimTex {
    uint32_t tex_hash;
    GFX::Texture tex;
};

struct Animation {
    uint32_t animname_hash;
    int fps;
    std::vector<int> indices;
};

struct Anim_obj {
    Animation *curanim;
    KeyFrame *curkeyframe;
    AnimTex *curtex;
    Tween<float, LinearEasing> curframe;
    uint32_t curanimhash;
};

class Object {
public:
    void init(std::string path);
    void update(float animtime);
    void setAnim(uint32_t animhash);
    void playAnim(float endtime);
    bool playing(void) {return !(obj.curframe.getValue()>=getCurIndicieCount());}
    float getCurIndicieCount(void) {return obj.curanim->indices.size()-1;}
    float getCurFps(void) {return obj.curanim->fps;}
    void setScale(float s) {scale = s;}
    void draw(GFX::Renderer *renderer, const GFX::XY<int32_t> &pos);
    void free(void);
private:
    std::vector<KeyFrame> keyframes;
    std::vector<AnimTex> textures;
    std::vector<Animation> animations;
    Anim_obj obj;
    float scale;
    int findAnimationIndex(uint32_t hash);
    AnimTex* findTexture(uint32_t hash);
};

class ObjectGeneric {
public:
    void init(std::string path) {obj2d.init(path); setLoop(false);}
    void setLoop(bool l) {loop = l;};
    
    void update(TIMER::Timer &timer) {obj2d.update(timer.elapsedMS()/1000); if (loop && !obj2d.playing()) playAnim();}
    void setAnim(uint32_t animhash) {obj2d.setAnim(animhash);}
    void playAnim(void) {obj2d.playAnim(obj2d.getCurIndicieCount()/obj2d.getCurFps());} //fps animation
    void playAnim(uint32_t animhash) { setAnim(animhash); playAnim();}
    void draw(GFX::Renderer *renderer, const GFX::XY<int32_t> &pos) {obj2d.draw(renderer, pos);}
    void setScale(float s) {obj2d.setScale(s);};
    void free(void) {obj2d.free();}
private:
    bool loop;
    Object obj2d;
};

}