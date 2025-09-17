#pragma once

namespace SCENE {

class Scene {
public:
    inline Scene(void) {}
    virtual void update(void) {}
    virtual void draw(void) {}
    virtual ~Scene(void) {}
};

void set(Scene *scn);

}