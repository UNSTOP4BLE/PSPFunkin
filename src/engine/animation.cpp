
#include "animation.hpp"
#include "hash.hpp"

namespace ANIMATION {

static std::string dirname(const std::string& path) {
    size_t pos = path.find_last_of("/");
    return path.substr(0, pos) + "/";
}

template<typename T>
void Animatable<T>::init(ASSETS::AssetManager &mgr, std::string path) {
    std::string dir = dirname(path);
    config = mgr.get<ASSETS::JsonAsset>(path.c_str());
    auto &jdata = config->value;

    //texture data
    for (int i = 0; i < jdata["textures"].size(); i++) {
        auto &curtex = jdata["textures"][i];
        AnimTex t {
            Hash::FromString(curtex.asString().c_str()), //tex_hash
            mgr.get<ASSETS::ImageAsset>((dir + curtex.asString()).c_str()) //texture
        };
        textures.push_back(t);
    }

    //frame data
    for (int i = 0; i < jdata["frames"].size(); i++) {
        auto &curframe = jdata["frames"][i];
        KeyFrame f {
            Hash::FromString(curframe[0].asString().c_str()), //tex_hash
            {
            curframe[1].asInt(), //x
            curframe[2].asInt(), //y
            curframe[3].asInt(), //w
            curframe[4].asInt()  //h 
            }, 
            curframe[5].asInt(), //ox
            curframe[6].asInt() //oy
        };
        keyframes.push_back(f);
    }
    //indices data
    for (int i = 0; i < jdata["indices"].size(); i++) {
        auto &curanim = jdata["indices"][i];
        Animation anim {
            Hash::FromString(curanim[0].asString().c_str()), //animname_hash
            curanim[1].asInt() //fps
        };
        for (auto &curind : curanim[2]) {
            anim.indices.push_back(curind.asInt());
        }
        animations.push_back(anim);
    }

//    mgr.release(json.path); do this in free

    //set initial animation
    playAnim(""_h);
}

template<typename T>
void Animatable<T>::update(float t) {
    curframe.setTweenTime(t);
    curkeyframe = &keyframes[curanim->indices[static_cast<int>(curframe.getValue())]];
    curtex = findTexture(curkeyframe->tex_h);
}

template<typename T>
void Animatable<T>::setAnim(uint32_t anim_h) {
    //set to initial frame of animation
    curanim = &animations[findAnimationIndex(anim_h)];
    curkeyframe = &keyframes[curanim->indices[0]];
    curtex = findTexture(curkeyframe->tex_h);
}

template<typename T>
void Animatable<T>::playAnim(void) {
    curframe.setValue(0, getIndicieCount(), timemanager.apply(getIndicieCount(), getIndicieCount()));
}

//helpers
template<typename T>
int Animatable<T>::findAnimationIndex(uint32_t hash) {
    for (int i = 0; i < animations.size(); i++) {
        if (animations[i].anim_h == hash)
            return static_cast<int>(i);
    }
    return 0; // not found, return first animation
}

template<typename T>
AnimTex *Animatable<T>::findTexture(uint32_t hash) {
    //todo add asssert here
    for (auto& tex : textures) {
        if (tex.tex_h == hash)
            return &tex;
    }
    return nullptr;
}

template class Animatable<TimeFPS>;
template class Animatable<TimeSTEP>;

} //namespace ANIMATION