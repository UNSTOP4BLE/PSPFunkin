#pragma once
#include <json/json.h>
#include "animation.h"

void loadJson(const char *filename, Json::Value *data);
std::vector<AnimFrames> readFramesFromJson(const char *filename);
std::string getPath(std::string path);