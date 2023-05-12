#include "stage.h"
#include "psp/file.h"

//misc functions
static void parseRect(Rect *retrect, Json::Value *rect);
static void parseObjects(std::vector<StageObject> &objs, const char *gnd, Json::Value data);
static void tickObjects(std::vector<StageObject> &objs, float cx, float cy);

//stage functions
void Stage::load(std::string path) {
    Json::Value data;
    loadJson(path.c_str(), &data);

    //textures
    for (int i = 0; i < (int)data["textures"].size(); i++) {
        textures[i].def = data["textures"][i]["def"].asString();
        textures[i].texture = g2dTexLoad(data["textures"][i]["path"].asString().c_str(), G2D_SWIZZLE);
    }

    //stage data
    parseObjects(fgobjects, "foreground", data);
    parseObjects(mdobjects, "middleground", data);
    parseObjects(bgobjects, "middleground", data);
}

void Stage::tick(float cx, float cy) {
    tickObjects(fgobjects, cx, cy);
    tickObjects(mdobjects, cx, cy);
    tickObjects(bgobjects, cx, cy);
}

void Stage::free(void) {
    for (int i = 0; i < (int)textures.size(); i++) {
        g2dTexFree(&textures[i].texture);
    }
}

void Stage::drawObjects(std::vector<StageObject> &objs, float camzoom) {
    for (int i = 0; i < (int)objs.size(); i++) {
        if (objs.size() == 0)
            break;
        int tex = -1;
        //check for correct texture
        for (int j = 0; j < (int)textures.size(); i++) {
            if (objs[i].def == textures[j].def)
                tex = j;
        }
        ASSERTFUNC(tex != -1, "invalid background texture def, is it defined?");
        DrawZoomG2DTex(textures[tex].texture, &objs[i].img, &objs[i].disp, false, objs[i].angle, objs[i].alpha, camzoom);
    }
}

//misc functions
static void parseRect(Rect *retrect, Json::Value *rect) {
    retrect->x = rect[0].asInt();
    retrect->y = rect[1].asInt();
    retrect->w = rect[2].asInt();
    retrect->h = rect[3].asInt();
}

static void parseObjects(std::vector<StageObject> &objs, const char *gnd, Json::Value data) {
    objs.resize(data["objects"][gnd].size());    
    for (int i = 0; i < (int)objs.size(); i++) {
        if (objs.size() == 0)
            break;
        objs[i].def = data["objects"]["foreground"][i]["def"].asString();
        parseRect(&objs[i].img, &data["objects"]["foreground"][i]["img"]);
        parseRect(&objs[i].initialdisp, &data["objects"]["foreground"][i]["disp"]);
        objs[i].scrollfactor = data["objects"]["foreground"][i]["scrollfactor"].asFloat();
    }
}

static void tickObjects(std::vector<StageObject> &objs, float cx, float cy) {
    for (int i = 0; i < (int)objs.size(); i++) {
        if (objs.size() == 0)
            break;

        objs[i].disp.x = objs[i].initialdisp.x - (cx * objs[i].scrollfactor);
        objs[i].disp.y = objs[i].initialdisp.y - (cy * objs[i].scrollfactor);
    }
}