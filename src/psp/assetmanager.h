#pragma once

#include <unordered_map>   
#include "gfx.h"
#include "audio_streamed_file.h"
#include "audio_buffer.h"
#include <json/json.h>

class Asset {
public:
    virtual ~Asset(void) {}

    static const Asset *loadFromFile(const char *path) { (void) path; return nullptr; }
};

class ImageAsset : public Asset {
public:
    std::string assetpath;
    GFX::Texture image;
    inline ImageAsset(void) {}
    ~ImageAsset(void);

    static const Asset *loadFromFile(const char *path);
};

class SoundAsset : public Asset {
public:
    std::string assetpath;
    Audio::AudioBuffer soundbuffer;
    inline SoundAsset(void) {}
    ~SoundAsset(void);

    static const Asset *loadFromFile(const char *path);
};

class JsonAsset : public Asset {
public:
    std::string assetpath;
    Json::Value value;
    inline JsonAsset(void) {}
    ~JsonAsset(void);

    static const Asset *loadFromFile(const char *path);
};
/* Asset manager */

class AssetTableEntry {
public:
    int refCount;
    const Asset *ptr;

    inline AssetTableEntry(void) : refCount(1), ptr(nullptr) {}
    inline ~AssetTableEntry(void) {
        if (ptr) {
            delete ptr;
        ptr = nullptr;
    }
}

};

class AssetManager {
private:
    std::unordered_map<uint32_t, AssetTableEntry> loadedAssets;

    const Asset *_get(const char *path, const Asset *(*loader)(const char *path));

public:
    // Allow loading new assets: assetManager.get<ImageAsset>(path)
    template<typename T> inline const T *get(const char *path) {
        return reinterpret_cast<const T *>(_get(path, &(T::loadFromFile)));
    }

    // Do not allow loading new assets: assetManager.get(path)
    inline const Asset *get(const char *path) {
        return _get(path, nullptr);
}

void release(const char *path);
};