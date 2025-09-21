#include "assets.hpp"
#include "hash.hpp"
#include "file.hpp"
#include <cassert>
#include <fstream>

namespace ASSETS {

//textures
const Asset *ImageAsset::loadFromFile(const char *path) {
    auto asset = new ImageAsset();
    asset->image = FS::loadTexFile(path);
    asset->assetpath = static_cast<std::string>(path);
    return asset;
}

ImageAsset::~ImageAsset(void) {}

//sound effects
const Asset *SoundAsset::loadFromFile(const char *path) {
    auto asset = new SoundAsset();
    AUDIO::loadFile(asset->soundbuffer, path);
    asset->assetpath = static_cast<std::string>(path);
    return asset;
}

SoundAsset::~SoundAsset(void) {}

//json
const Asset *JsonAsset::loadFromFile(const char *path) {
    auto asset = new JsonAsset();
    
    std::ifstream file(path);
    Json::Reader reader;

    assert(reader.parse(file, asset->value));   

    file.close();

    asset->assetpath = static_cast<std::string>(path);
    return asset;
}

JsonAsset::~JsonAsset(void) {}


/* Asset manager */
const Asset *AssetManager::_get(const char *path, const Asset *(*loader)(const char *path)) {
    auto pathHash = Hash::FromString(path);
    auto result = loadedAssets.find(pathHash);

    if (result == loadedAssets.end()) {
        // New asset, load data
        if (!loader)
            return nullptr;

        auto ptr = loader(path);

        if (!ptr)
            return nullptr; // Loading failed

        loadedAssets[pathHash].ptr = ptr; // Create new entry
        return ptr;
    } else {
        // Already loaded asset
        auto &asset = result->second;
        asset.refCount++;
        return asset.ptr;
    }
}

void AssetManager::release(const char *path) {
    auto pathHash = Hash::FromString(path);
    auto result = loadedAssets.find(pathHash);

    std::string errmsg = "Attempted to release asset that was not loaded " + static_cast<std::string>(path);
    assert(result != loadedAssets.end());

    auto tableEntry = &(result->second);
    tableEntry->refCount--;
    if (tableEntry->refCount <= 0)
        loadedAssets.erase(result);
}

void AssetManager::printLoadedAssets(void) {
    printf("Currently loaded assets:\n");

    for (auto item = loadedAssets.begin(); item != loadedAssets.end(); item++) {
        auto &asset = item->second;
        printf("- [%d] %s\n", asset.refCount, asset.ptr->assetpath.c_str());
    }
}

}