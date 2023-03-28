#pragma once
#include <SDL2/SDL.h>
#include <cstdint>       
#include <vector>      
class AudioBuffer {
public:

    inline int getNumSamples(void) {
        return data.size() / (channels * SDL_AUDIO_BITSIZE(format) / 8);
    }
    std::vector<uint8_t> data; 
    uint16_t channels;
    uint32_t samplerate;
    SDL_AudioFormat format;
};