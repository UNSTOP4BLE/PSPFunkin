#include "stage.h"
#include "main.h"
#include "psp/file.h"

//misc functions
static void parseObjects(std::vector<StageObject> &objs, std::string gnd, Json::Value data);
static void tickObjects(std::vector<StageObject> &objs, float cx, float cy);

//stage functions
void Stage::load(const char *jpath, std::string stage) {
    Json::Value data;
    loadJson(jpath, &data);

    //textures
    textures.resize(static_cast<int>(data["textures"].size()));
    for (int i = 0; i < static_cast<int>(data["textures"].size()); i++) {
        if (data["textures"].size() == 0)
            return;
        textures[i].def = "";
        textures[i].def = data["textures"][i]["def"].asString();
        std::string texpath = "assets/stages/" + stage + "/" + data["textures"][i]["path"].asString();
        textures[i].texture = app->assetmanager.get<ImageAsset>(texpath.c_str()); 
    }

    //stage data
    parseObjects(fgobjects, "foreground", data);
    parseObjects(mdobjects, "middleground", data);
    parseObjects(bgobjects, "background", data);

    debugLog("Stage::load: %s", jpath);
}

void Stage::tick(float cx, float cy) {
    tickObjects(fgobjects, cx, cy);
    tickObjects(mdobjects, cx, cy);
    tickObjects(bgobjects, cx, cy);
}

void Stage::drawObjects(std::vector<StageObject> &objs, float camzoom) {
    for (int i = 0; i < static_cast<int>(objs.size()); i++) {
        if (objs.size() == 0)
            return;
        int tex = -1;
        //check for correct texture
        for (int j = 0; j < static_cast<int>(textures.size()); j++) {
            if (objs[i].def == textures[j].def) {
                tex = j;
                break;
            }
        }
        ASSERTFUNC(tex != -1, "invalid background texture def, is it defined?");
        GFX::drawTex<float>(&textures[tex].texture->image, &objs[i].img, &objs[i].disp, objs[i].angle, objs[i].alpha, camzoom);
    }
}

//misc functions
static void parseObjects(std::vector<StageObject> &objs, std::string gnd, Json::Value data) {
    objs.resize(data["objects"][gnd].size());   
    for (int i = 0; i < static_cast<int>(data["objects"][gnd].size()); i++) {
        if (data["objects"][gnd].size() == 0)
            return;
        objs[i].def = data["objects"][gnd][i]["def"].asString();

        //parse img
        objs[i].img.x = data["objects"][gnd][i]["img"][0].asInt();
        objs[i].img.y = data["objects"][gnd][i]["img"][1].asInt();
        objs[i].img.w = data["objects"][gnd][i]["img"][2].asInt();
        objs[i].img.h = data["objects"][gnd][i]["img"][3].asInt();
        //parse disp
        objs[i].initialdisp.x = data["objects"][gnd][i]["disp"][0].asInt();
        objs[i].initialdisp.y = data["objects"][gnd][i]["disp"][1].asInt();
        objs[i].disp.w = objs[i].initialdisp.w = data["objects"][gnd][i]["disp"][2].asInt();
        objs[i].disp.h = objs[i].initialdisp.h = data["objects"][gnd][i]["disp"][3].asInt();

        //parse other
        objs[i].scrollfactor[0] = data["objects"][gnd][i]["scrollfactor"][0].asFloat();
        objs[i].scrollfactor[1] = data["objects"][gnd][i]["scrollfactor"][1].asFloat();
        objs[i].angle = data["objects"][gnd][i]["angle"].asInt();
        objs[i].alpha = data["objects"][gnd][i]["alpha"].asInt();
    }
}

static void tickObjects(std::vector<StageObject> &objs, float cx, float cy) {
    for (int i = 0; i < static_cast<int>(objs.size()); i++) {
        if (objs.size() == 0)
            return;

        objs[i].disp.x = objs[i].initialdisp.x - (cx * objs[i].scrollfactor[0]);
        objs[i].disp.y = objs[i].initialdisp.y - (cy * objs[i].scrollfactor[1]);
    }
}