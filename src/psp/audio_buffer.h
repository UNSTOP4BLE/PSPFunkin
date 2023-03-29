#pragma once

#include <cstdint>
#include <vector>
#include <SDL2/SDL_audio.h>
#include "../main.h"

namespace Audio {

class AudioBuffer {
public:
    inline int getBytesPerSample(void) {
        return channels * SDL_AUDIO_BITSIZE(format) / 8;
    }
    inline int getNumSamples(void) {
        return data.size() / getBytesPerSample();
    }

    std::vector<uint8_t> data; 
    uint16_t channels;
    uint32_t samplerate;
    SDL_AudioFormat format;
};

}
