#include "../main.h"
#include "../app.h"
#include "assetmanager.h"
#include "hash.h"

Asset *AssetManager::getAsset(const char *path, bool allowLoading) {
    auto pathHash = Hash::FromString(path);
    auto result = assets.find(pathHash);

    if (result == assets.end()) {
        // new asset, load data
        if (!allowLoading)
            return nullptr;

        auto asset = &assets[pathHash]; // create entry
//        loadAssetData(asset, path);
        return asset;
    } else {
        // already loaded asset
        auto asset = &(result->second);
        asset->refCount++;
        return asset;
    }
}

void AssetManager::release(const char *path) {
    auto pathHash = Hash::FromString(path);
    auto result = assets.find(pathHash);

    ASSERTFUNC(result != assets.end(), "attempted to release asset that was not loaded");

    auto asset = &(result->second);
    asset->refCount--;
    if (asset->refCount <= 0)
        assets.erase(result);
}