#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <fstream>
#include <iostream>
#include "../utils/ThreadPool.hpp"

using namespace std;
using namespace sf;


/**
 * AssetManager - Singleton texture/icon loading and caching system (header-only)
 *
 * Features:
 * - On-demand background texture loading using ThreadPool (doesn't block main game loop)
 * - O(1) lookup performance with insertion order preservation
 * - Thread-safe two-phase loading (file I/O in background, SFML creation on main thread)
 * - Automatic resource caching and sharing to prevent duplicate loads
 *
 * Usage:
 *   // Request texture to load in background
 *   AssetManager::getInstance().loadTexture("tile000.png");
 *
 *   // In game loop - process loaded textures (call each frame)
 *   AssetManager::getInstance().update();
 *
 *   // Retrieve loaded texture (returns nullptr if not ready yet)
 *   auto texture = AssetManager::getInstance().getTexture("tile000.png");
 *   if (texture) {
 *       sprite.setTexture(*texture);
 *   }
 */
class AssetManager {
    ThreadPool loadingPool;
    unordered_map<string, shared_ptr<Texture>> textureCache;
    vector<string> textureOrder;
    size_t totalTextureCount;
    struct PendingAsset {
        string key;             // Asset identifier (filename)
        vector<char> fileData;  // Raw file bytes loaded in background
    };
    queue<PendingAsset> pendingAssets;
    mutex pendingMutex;         // Protects pendingAssets queue

    AssetManager()
        : loadingPool(thread::hardware_concurrency()),
          textureCache(),
          textureOrder(),
          totalTextureCount(0),
          pendingAssets(),
          pendingMutex() {
    }

public:
    static auto getInstance() -> AssetManager& {
        static auto instance = AssetManager();
        return instance;
    }

    // Delete copy/move constructors (singleton)
    AssetManager(const AssetManager&) = delete;
    auto operator=(const AssetManager&) -> AssetManager& = delete;
    AssetManager(AssetManager&&) = delete;
    auto operator=(AssetManager&&) -> AssetManager& = delete;

    /**
     * Request a texture to be loaded asynchronously in background
     * Safe to call multiple times with same filename (will only load once)
     * @param filename Texture filename relative to assets/images/icons/
     */
    void loadTexture(const string& filename) {
        // Skip if already loaded or pending
        if (this->isTextureLoaded(filename)) {
            return;
        }

        this->totalTextureCount++;

        // Enqueue background task to load texture file data
        this->loadingPool.enqueue([this, filename]() {
            auto fullPath = "assets/images/icons/" + filename;

            // Read file into memory (runs on background thread)
            auto file = ifstream(fullPath, ios::binary | ios::ate);
            if (!file.is_open()) {
                cerr << "[AssetManager] Failed to open texture: " << fullPath << endl;
                return;
            }

            auto size = file.tellg();
            file.seekg(0, ios::beg);

            auto buffer = vector<char>(size);
            if (!file.read(buffer.data(), size)) {
                cerr << "[AssetManager] Failed to read texture: " << fullPath << endl;
                return;
            }

            // Add to pending queue (will be processed on main thread)
            {
                auto lock = lock_guard<mutex>(this->pendingMutex);
                this->pendingAssets.push({filename, move(buffer)});
            }

            cout << "[AssetManager] Loaded texture data: " << filename << " (" << size << " bytes)" << endl;
        });
    }

    /**
     * Process pending assets loaded in background threads
     * MUST be called each frame from main thread to finalize SFML resources
     */
    void update() {
        this->processPendingAssets();
    }

    auto getTexture(const string& name) -> shared_ptr<Texture> {
        auto it = this->textureCache.find(name);
        return it != this->textureCache.end()
            ? it->second
            : nullptr;
    }

    auto isTextureLoaded(const string& name) const -> bool {
        return this->textureCache.find(name) != this->textureCache.end();
    }

    auto getTextureNames() const -> const vector<string>& {
        return this->textureOrder;
    }

    auto getLoadedTextureCount() const { 
        return this->textureCache.size(); 
    }

    auto getPendingAssetCount() const {
        auto lock = lock_guard<mutex>(const_cast<mutex&>(this->pendingMutex));
        return this->pendingAssets.size();
    }

    auto getTotalTextureCount() const { 
        return this->totalTextureCount; 
    }

    auto getTotalAssetCount() const { 
        return this->totalTextureCount; 
    }

    auto getLoadedAssetCount() const { 
        return this->textureCache.size(); 
    }

    auto isLoadingComplete() const -> bool {
        return this->getLoadedAssetCount() == this->getTotalAssetCount();
    }

    auto getLoadingProgress() const -> float {
        auto total = this->getTotalAssetCount();
        if (total == 0) return 1.0f;
        return static_cast<float>(this->getLoadedAssetCount()) / static_cast<float>(total);
    }

private:
    void processPendingAssets() {
        // Process all pending textures loaded in background threads
        // This MUST run on the main thread (SFML OpenGL context requirement)
        auto lock = lock_guard<mutex>(this->pendingMutex);

        while (!this->pendingAssets.empty()) {
            auto& pending = this->pendingAssets.front();
            auto texture = make_shared<Texture>();

            // Create SFML texture from loaded data (main thread only!)
            if (!texture->loadFromMemory(pending.fileData.data(), pending.fileData.size()))
                cerr << "[AssetManager] Failed to create texture from data: " << pending.key << endl;
            else {
                this->textureCache[pending.key] = texture;
                this->textureOrder.push_back(pending.key);
                cout << "[AssetManager] Finalized texture: " << pending.key << endl;
            }
            this->pendingAssets.pop();
        }
    }
};
