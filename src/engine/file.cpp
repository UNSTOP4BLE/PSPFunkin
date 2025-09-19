#include "file.hpp"
#include <cassert>
#include <unistd.h> 
#include <fstream>
#ifndef PSP
#include "pc/renderersdl.hpp"
#endif

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#endif

namespace FS {
GFX::Texture loadTexFile(const std::string& path) {
#ifdef PSP
    
#else
    GFX::Texture tex;
    SDL_Surface* surface = IMG_Load(path.c_str());
    assert(surface);

    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    tex.tex = SDL_CreateTextureFromSurface(GFX::g_sdlrenderer, converted);
    SDL_FreeSurface(converted);

    assert(tex.tex);
    SDL_SetTextureBlendMode(tex.tex, SDL_BLENDMODE_BLEND);

    return tex;

#endif
    //default
    return GFX::Texture();
}

static std::string getAppDirectory() {
    std::string path;

#if defined(_WIN32)
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (length == 0) return "";
    path = std::string(buffer, length);

#elif defined(__APPLE__)
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) return "";
    char resolved[PATH_MAX];
    realpath(buffer, resolved);
    path = resolved;

#elif defined(__linux__)
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    if (count == -1) return "";
    buffer[count] = '\0';
    path = buffer;

#else
    // Fallback for unknown platforms (e.g., PSP)
    return "";
#endif

    // Remove executable name to get directory
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        path = path.substr(0, lastSlash);
    }

    return path;
}

std::string getFilePath(const std::string& path) {
    const std::string prefix = getAppDirectory() + "/";
    printf("path %s\n", (prefix + path).c_str());
    assert(access((prefix + path).c_str(), F_OK) == 0);
    return prefix + path;
}

Json::Value readJsonFile(const std::string& path) {
    std::ifstream file(path);
    Json::Reader reader;
    Json::Value jdata;
    int r = reader.parse(file, jdata);
    assert(r);   
    file.close();
    return jdata;
}
}