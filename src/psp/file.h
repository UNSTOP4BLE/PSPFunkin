#pragma once
#include <json/json.h>
#include "types.h"

void loadJson(const char *filename, Json::Value *data);
std::vector<AnimFrames> readFramesFromJson(const char *filename);
std::vector<std::vector<int>> readConfFromJson(const char *filename);