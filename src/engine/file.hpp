#pragma once
#include "renderer.hpp"
#include <string>
#include <json/json.h>

namespace FS {
GFX::Texture loadTexFile(const std::string& path);
std::string getFilePath(const std::string& path);
Json::Value readJsonFile(const std::string& path);

}