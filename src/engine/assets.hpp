#pragma once

#include <string>
#include <unordered_map>

#include "renderer.hpp"
#include "audio_streamed_file.hpp"
#include "audio_buffer.hpp"
#include <json/json.h>

namespace ASSETS {

class Asset {
public:
    std::string assetpath;
    virtual ~Asset() = default;

    static const Asset* loadFromFile(const char* path) { (void)path; return nullptr; }
};

class ImageAsset : public Asset {
public:
    GFX::Texture image;

    ImageAsset() = default;
    ~ImageAsset();

    static const Asset* loadFromFile(const char* path);
};

class SoundAsset : public Asset {
public:
    AUDIO::AudioBuffer soundbuffer;

    SoundAsset() = default;
    ~SoundAsset();

    static const Asset* loadFromFile(const char* path);
};

class JsonAsset : public Asset {
public:
    Json::Value value;

    JsonAsset() = default;
    ~JsonAsset();

    static const Asset* loadFromFile(const char* path);
};

class AssetTableEntry {
public:
    int refCount;
    const Asset* ptr;

    AssetTableEntry() : refCount(1), ptr(nullptr) {}
    ~AssetTableEntry() {
        if (ptr) 
            delete ptr;
        ptr = nullptr;
    }
};

class AssetManager {
private:
    std::unordered_map<uint32_t, AssetTableEntry> loadedAssets;

    const Asset* _get(const char* path, const Asset* (*loader)(const char*));

public:
    // Allow loading new assets: assetManager.get<ImageAsset>(path)
    template<typename T>
    const T* get(const char* path) {
        return reinterpret_cast<const T*>(_get(path, &(T::loadFromFile)));
    }

    // Only retrieve already loaded assets
    const Asset* get(const char* path) {
        return _get(path, nullptr);
    }

    void release(const char* path);
    void printLoadedAssets();
};

} // namespace ASSETMANAGER
