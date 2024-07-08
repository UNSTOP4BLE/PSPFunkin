#pragma once
#include <json/json.h>
#include "animation.h"

std::vector<AnimFrames> readFramesFromJson(const char *filename);
std::string getPath(std::string path);