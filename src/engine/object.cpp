#include "object.hpp"

#include <json/json.h>
#include <fstream>
#include <cassert>
#include "hash.hpp"
#include "../app.hpp"
#include "renderer.hpp"

namespace OBJECT {

static void splitPath(const std::string& path, std::string& directory, std::string& filename) {
    // Look for last forward slash or backslash
    size_t pos = path.find_last_of("/\\");
    
    if (pos == std::string::npos) {
        // No separator found → path is just the filename
        directory = "";
        filename = path;
    } else {
        directory = path.substr(0, pos) + "/";       // everything before last slash
        filename  = path.substr(pos + 1);      // everything after last slash
    }
}

void Object::init(std::string path) {
    std::string dir, jsonname;
    splitPath(path, dir, jsonname);
    std::ifstream file(dir + jsonname);
    Json::Reader reader;
    Json::Value jdata;
    assert(reader.parse(file, jdata));   

    //texture data
    for (int i = 0; i < jdata["textures"].size(); i++) {
        auto &curtex = jdata["textures"][i];
        AnimTex t {
            Hash::FromString(curtex.asString().c_str()), //tex_hash
            FS::loadTexFile(dir + curtex.asString()) //texture
        };
        textures.push_back(t);
    }

    //frame data
    for (int i = 0; i < jdata["frames"].size(); i++) {
        auto &curframe = jdata["frames"][i];
        KeyFrame f {
            Hash::FromString(curframe[0].asString().c_str()), //tex_hash
            curframe[1].asInt(), //x
            curframe[2].asInt(), //y
            curframe[3].asInt(), //w
            curframe[4].asInt(), //h 
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
    file.close();

    //set animation as default 
    setAnim("none"_h); 
    obj.curframe.setValue(0);//end immediatley
    setScale(1);
}

void Object::update(float animtime) {
    obj.curframe.setTweenTime(animtime);
    obj.curkeyframe = &keyframes[obj.curanim->indices[static_cast<int>(obj.curframe.getValue())]];
    obj.curtex = findTexture(obj.curkeyframe->tex_hash);
}

int Object::findAnimationIndex(uint32_t hash) {
    auto it = std::find_if(animations.begin(), animations.end(),
        [hash](const Animation& anim) { return anim.animname_hash == hash; });

    if (it != animations.end()) {
        return static_cast<int>(std::distance(animations.begin(), it));
    }

    return 0; // Not found
}

AnimTex* Object::findTexture(uint32_t hash) {
    auto it = std::find_if(textures.begin(), textures.end(),
                           [hash](const AnimTex& t) { return t.tex_hash == hash; });

    if (it != textures.end()) {
        return &(*it); // return pointer to found texture
    }

    return nullptr; // not found
}

void Object::setAnim(uint32_t animhash) {
    obj.curanimhash = animhash;
    //set to initial frame of animation
    obj.curanim = &animations[findAnimationIndex(animhash)];
    obj.curkeyframe = &keyframes[obj.curanim->indices[0]];
    obj.curtex = findTexture(obj.curkeyframe->tex_hash);
}

void Object::playAnim(float endtime) {
//    setAnim(obj.curanimhash); reset animation
    obj.curframe.setValue(0, getCurIndicieCount(), endtime);
}

void Object::draw(GFX::XY<int32_t> pos) {
    auto &f = obj.curkeyframe;

    //screen center
    int32_t cx = GFX::SCREEN_WIDTH / 2;
    int32_t cy = GFX::SCREEN_HEIGHT / 2;

    //calculate destination rect so scaling is around screen center
    int32_t dst_x = cx + (pos.x - cx) * scale - f->ox * scale;
    int32_t dst_y = cy + (pos.y - cy) * scale - f->oy * scale;
    int32_t scaled_w = static_cast<int32_t>(f->w * scale);
    int32_t scaled_h = static_cast<int32_t>(f->h * scale);

    GFX::RECT<int32_t> src = {f->x, f->y, f->w, f->h};
    GFX::RECT<int32_t> dst = {dst_x, dst_y, scaled_w, scaled_h};

    g_app.renderer->drawTexRect(obj.curtex->tex, src, dst, 0, 0xFFFFFFFF);
}

void Object::free() {
    //todo
}

}