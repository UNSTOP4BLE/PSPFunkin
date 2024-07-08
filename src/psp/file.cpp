#include "../main.h"
#include "file.h"
#include <fstream>
#include "../screen.h"
#include "../app.h"

std::vector<AnimFrames> readFramesFromJson(const char *filename)
{
    //load the json file 
    const JsonAsset *data = app->assetmanager.get<JsonAsset>(getPath(filename).c_str());

    //push frame data to the vector array
    std::vector<AnimFrames> frames(data->value["frames"].size());

    for (int i = 0; i < static_cast<int>(data->value["frames"].size()); i++)
    {
        frames[i].tex = data->value["frames"][i][0].asInt(); //tex
        frames[i].x = data->value["frames"][i][1].asInt(); // x
        frames[i].y = data->value["frames"][i][2].asInt(); // y
        frames[i].w = data->value["frames"][i][3].asInt(); // w
        frames[i].h = data->value["frames"][i][4].asInt(); // h
        frames[i].offsetx = data->value["frames"][i][5].asInt(); // offset x 
        frames[i].offsety = data->value["frames"][i][6].asInt(); // offset y
    }
    app->assetmanager.release(data->assetpath.c_str());
    return frames;
}

std::string getPath(std::string path) {
    #ifdef __vita__ 
        return "app0:/" + path;
    #else
        return path;
    #endif
}