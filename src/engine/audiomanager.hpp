//done
#pragma once

#include "audio_streamed_file.hpp"

namespace MANAGER {

struct AudioFile {
    std::unique_ptr<AUDIO::StreamedFile> stream;
    std::string path;
};

class AudioManager {
public:
//    AudioManager() : stream(nullptr) {} //todo add this to other classes to prevent shit going wrong, define a pointer as null

    void loadFile(AUDIO::Mixer &mixer, const std::string& path);
    void update(void);
    void play(bool loop);
    float getMS(void);

private:
    AudioFile file;
};

}