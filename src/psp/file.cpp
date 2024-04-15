#include "../main.h"
#include "file.h"
#include <fstream>
#include "../screen.h"

void loadJson(const char *filename, Json::Value *data) 
{
    std::ifstream file(filename);
    Json::Reader reader;
 
    ASSERTFUNC(reader.parse(file, *data), reader.getFormattedErrorMessages().c_str());   

    file.close();
}

std::vector<AnimFrames> readFramesFromJson(const char *filename)
{
    //load the json file 
    Json::Value data;
    loadJson(filename, &data);

    //push frame data to the vector array
    std::vector<AnimFrames> frames(data["frames"].size());

    for (int i = 0; i < static_cast<int>(data["frames"].size()); i++)
    {
        frames[i].tex = data["frames"][i][0].asInt(); //tex
        frames[i].x = data["frames"][i][1].asInt(); // x
        frames[i].y = data["frames"][i][2].asInt(); // y
        frames[i].w = data["frames"][i][3].asInt(); // w
        frames[i].h = data["frames"][i][4].asInt(); // h
        frames[i].offsetx = data["frames"][i][5].asInt(); // offset x 
        frames[i].offsety = data["frames"][i][6].asInt(); // offset y
    }
    return frames;
}

std::string getPath(std::string path) {
    #ifdef __vita__ 
        return "app0:/" + path;
    #else
        return path;
    #endif
}