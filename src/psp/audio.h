#pragma once
#include "../main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>      
#include <stdbool.h> 

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

void Audio_Init(void);

AudioBuffer *Audio_LoadFile(const char *path);