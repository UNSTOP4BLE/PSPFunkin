#pragma once
#include "renderer.hpp"
#include <string>
namespace FS {
GFX::Texture loadTexFile(const std::string& path);
std::string getFilePath(std::string path);
}