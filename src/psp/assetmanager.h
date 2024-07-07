#pragma once

#include <unordered_map>   
#include "gfx.h"
#include "audio_streamed_file.h"
#include "audio_buffer.h"

class Asset {
public:
    int refCount;
    union {
        const void *ptr;

        const GFX::Texture *texture;
        const Audio::StreamedFile *stream;
        const Audio::AudioBuffer *sound;
        /* ... */
    };

    Asset(void) : refCount(1), ptr(nullptr) {}
    ~Asset(void) {
        if (ptr) { /* deallocate asset data here */ }
    }
};

class AssetManager {
private:
    std::unordered_map<uint32_t, Asset> assets;

public:
    Asset *getAsset(const char *path, bool allowLoading = true);
    void release(const char *path);
};
