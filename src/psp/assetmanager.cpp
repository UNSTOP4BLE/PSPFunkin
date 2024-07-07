#include "../main.h"
#include "../app.h"
#include "assetmanager.h"
#include "hash.h"
#include <SDL2/SDL_image.h>

/* Asset classes */
const Asset *ImageAsset::loadFromFile(const char *path) {
    auto asset = new ImageAsset();
    asset->image.load(path);
    return asset;
 }

ImageAsset::~ImageAsset(void) {}
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

    ASSERTFUNC(result != loadedAssets.end(), "Attempted to release asset that was not loaded");

    auto tableEntry = &(result->second);
    tableEntry->refCount--;
    if (tableEntry->refCount <= 0)
        loadedAssets.erase(result);
}