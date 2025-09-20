//done
#pragma once

#include <cstdint>
#include <vector>
#include <SDL2/SDL_audio.h>

namespace AUDIO {

class AudioBuffer {
public:
    inline int getBytesPerSample(void) {
        return channels * SDL_AUDIO_BITSIZE(format) / 8;
    }
    inline int getNumSamples(void) {
        return data.size() / getBytesPerSample();
    }

    std::vector<uint8_t> data; 
    int channels, sampleRate;
    SDL_AudioFormat format;
};

}
