#include "scene.hpp"
#include "../app.hpp"

namespace SCENE {

void set(Scene *scn) {   
    if (g_app.curscene != nullptr)
        delete g_app.curscene;
    g_app.curscene = scn;
}

}