#pragma once 
#include "engine/audio_mixer.hpp"
#include "engine/scene.hpp"
#include "engine/renderer.hpp"
#include "engine/file.hpp"

class PSPFunkin {
public:
    GFX::Renderer *renderer;
    float deltatime;
//    Input::Event event;
    Audio::Mixer audiomixer;
    SCENE::Scene *curscene;
};

extern PSPFunkin g_app;

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#endif

inline std::string getAppDirectory() {
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

inline std::string getFilePath(std::string path) {
    const std::string prefix = getAppDirectory() + "/";
    //if psp change prefix bla bla
    
    return prefix + path;
}
