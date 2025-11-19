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
 * AssetManager - Singleton asset loading and caching system (header-only)
 *
 * Features:
 * - Background asset loading using ThreadPool (doesn't block main game loop)
 * - O(1) lookup performance with insertion order preservation
 * - Thread-safe two-phase loading (file I/O in background, SFML creation on main thread)
 * - Automatic resource caching and sharing to prevent duplicate loads
 *
 * Usage:
 *   AssetManager::getInstance().preloadAllAssets();  // Start background loading
 *   AssetManager::getInstance().update();            // Call each frame
 *   auto texture = AssetManager::getInstance().getTexture("tile.png");
 */
class AssetManager {
    // Thread pool for background file loading
    ThreadPool loadingPool;

    // Fast O(1) lookup caches
    unordered_map<string, shared_ptr<Texture>> textureCache;
    unordered_map<string, shared_ptr<Font>> fontCache;

    // Insertion order preservation (for iteration)
    vector<string> textureOrder;
    vector<string> fontOrder;

    // Total asset counts (for progress tracking)
    size_t totalTextureCount;
    size_t totalFontCount;

    // Staging area for background-loaded file data
    struct PendingAsset {
        string key;           // Asset identifier (filename)
        vector<char> fileData; // Raw file bytes loaded in background
        enum Type { TEXTURE, FONT } type;
    };
    queue<PendingAsset> pendingAssets;
    mutex pendingMutex;  // Protects pendingAssets queue

    AssetManager()
        : loadingPool(thread::hardware_concurrency()),
          textureCache(),
          fontCache(),
          textureOrder(),
          fontOrder(),
          totalTextureCount(0),
          totalFontCount(0),
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
     * Start loading all known assets in background
     * Call this once during game initialization
     */
    void preloadAllAssets() {
        // Preload all known assets used by the game
        // Assets are discovered from current codebase usage

        // Textures
        this->loadTextureAsync("tile000.png");

        // Fonts (only load once, shared by all entities)
        this->loadFontAsync("sansation.ttf");

        cout << "[AssetManager] Started background loading of assets..." << endl;
    }

    /**
     * Process pending assets loaded in background threads
     * MUST be called each frame from main thread to finalize SFML resources
     */
    void update() {
        // Process any assets that finished loading in background
        this->processPendingAssets();
    }

    /**
     * Get a texture by name (returns nullptr if not loaded yet)
     * @param name Filename relative to assets/images/ directory
     * @return Shared pointer to texture, or nullptr if not ready
     */
    auto getTexture(const string& name) -> shared_ptr<Texture> {
        auto it = this->textureCache.find(name);
        return it != this->textureCache.end()
            ? it->second
            : nullptr;
    }

    /**
     * Get a font by name (returns nullptr if not loaded yet)
     * @param name Filename relative to assets/fonts/ directory
     * @return Shared pointer to font, or nullptr if not ready
     */
    auto getFont(const string& name) -> shared_ptr<Font> {
        auto it = this->fontCache.find(name);
        return it != this->fontCache.end()
            ? it->second
            : nullptr;
    }

    auto isTextureLoaded(const string& name) const -> bool {
        return this->textureCache.find(name) != this->textureCache.end();
    }

    auto isFontLoaded(const string& name) const -> bool {
        return this->fontCache.find(name) != this->fontCache.end();
    }

    auto getTextureNames() const -> const vector<string>& { 
        return this->textureOrder; 
    }

    auto getFontNames() const -> const vector<string>& { 
        return this->fontOrder; 
    }

    auto getLoadedTextureCount() const -> size_t { return this->textureCache.size(); }
    auto getLoadedFontCount() const -> size_t { return this->fontCache.size(); }
    auto getPendingAssetCount() const -> size_t {
        auto lock = lock_guard<mutex>(const_cast<mutex&>(this->pendingMutex));
        return this->pendingAssets.size();
    }

    auto getTotalTextureCount() const -> size_t { return this->totalTextureCount; }
    auto getTotalFontCount() const -> size_t { return this->totalFontCount; }
    auto getTotalAssetCount() const -> size_t { return this->totalTextureCount + this->totalFontCount; }
    auto getLoadedAssetCount() const -> size_t { return this->textureCache.size() + this->fontCache.size(); }

    auto isLoadingComplete() const -> bool {
        return this->getLoadedAssetCount() == this->getTotalAssetCount();
    }

    auto getLoadingProgress() const -> float {
        auto total = this->getTotalAssetCount();
        if (total == 0) return 1.0f;
        return static_cast<float>(this->getLoadedAssetCount()) / static_cast<float>(total);
    }

    auto getLoadingProgressPercent() const -> int {
        return static_cast<int>(this->getLoadingProgress() * 100.0f);
    }

private:
    void loadTextureAsync(const string& filename) {
        this->totalTextureCount++;  // Track total assets queued

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
                this->pendingAssets.push({filename, move(buffer), PendingAsset::TEXTURE});
            }

            cout << "[AssetManager] Loaded texture data: " << filename << " (" << size << " bytes)" << endl;
        });
    }

    void loadFontAsync(const string& filename) {
        this->totalFontCount++;  // Track total assets queued

        // Enqueue background task to load font file data
        this->loadingPool.enqueue([this, filename]() {
            auto fullPath = "assets/fonts/" + filename;

            // Read file into memory (runs on background thread)
            auto file = ifstream(fullPath, ios::binary | ios::ate);
            if (!file.is_open()) {
                cerr << "[AssetManager] Failed to open font: " << fullPath << endl;
                return;
            }

            auto size = file.tellg();
            file.seekg(0, ios::beg);

            auto buffer = vector<char>(size);
            if (!file.read(buffer.data(), size)) {
                cerr << "[AssetManager] Failed to read font: " << fullPath << endl;
                return;
            }

            // Add to pending queue (will be processed on main thread)
            {
                auto lock = lock_guard<mutex>(this->pendingMutex);
                this->pendingAssets.push({filename, move(buffer), PendingAsset::FONT});
            }

            cout << "[AssetManager] Loaded font data: " << filename << " (" << size << " bytes)" << endl;
        });
    }

    void processPendingAssets() {
        // Process all pending assets loaded in background threads
        // This MUST run on the main thread (SFML OpenGL context requirement)

        auto lock = lock_guard<mutex>(this->pendingMutex);

        while (!this->pendingAssets.empty()) {
            auto& pending = this->pendingAssets.front();

            if (pending.type == PendingAsset::TEXTURE) {
                // Create SFML texture from loaded data (main thread only!)
                auto texture = make_shared<Texture>();
                if (texture->loadFromMemory(pending.fileData.data(), pending.fileData.size())) {
                    this->textureCache[pending.key] = texture;
                    this->textureOrder.push_back(pending.key);
                    cout << "[AssetManager] Finalized texture: " << pending.key << endl;
                } else {
                    cerr << "[AssetManager] Failed to create texture from data: " << pending.key << endl;
                }
            } else if (pending.type == PendingAsset::FONT) {
                // Create SFML font from loaded data (main thread only!)
                auto font = make_shared<Font>();
                if (font->loadFromMemory(pending.fileData.data(), pending.fileData.size())) {
                    this->fontCache[pending.key] = font;
                    this->fontOrder.push_back(pending.key);
                    cout << "[AssetManager] Finalized font: " << pending.key << endl;
                } else {
                    cerr << "[AssetManager] Failed to create font from data: " << pending.key << endl;
                }
            }

            this->pendingAssets.pop();
        }
    }
};
