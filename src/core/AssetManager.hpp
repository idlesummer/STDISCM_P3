#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

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
private:
    // Singleton - Private constructor
    AssetManager() : loadingPool(4) {
        // Initialize thread pool with 4 worker threads for background loading
    }

    // Thread pool for background file loading
    ThreadPool loadingPool;

    // Fast O(1) lookup caches
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textureCache;
    std::unordered_map<std::string, std::shared_ptr<sf::Font>> fontCache;

    // Insertion order preservation (for iteration)
    std::vector<std::string> textureOrder;
    std::vector<std::string> fontOrder;

    // Staging area for background-loaded file data
    struct PendingAsset {
        std::string key;           // Asset identifier (filename)
        std::vector<char> fileData; // Raw file bytes loaded in background
        enum Type { TEXTURE, FONT } type;
    };
    std::queue<PendingAsset> pendingAssets;
    std::mutex pendingMutex;  // Protects pendingAssets queue

    // Internal helper methods
    void loadTextureAsync(const std::string& filename) {
        // Enqueue background task to load texture file data
        loadingPool.enqueue([this, filename]() {
            std::string fullPath = "assets/images/icons/" + filename;

            // Read file into memory (runs on background thread)
            std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                std::cerr << "[AssetManager] Failed to open texture: " << fullPath << std::endl;
                return;
            }

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            if (!file.read(buffer.data(), size)) {
                std::cerr << "[AssetManager] Failed to read texture: " << fullPath << std::endl;
                return;
            }

            // Add to pending queue (will be processed on main thread)
            {
                std::lock_guard<std::mutex> lock(pendingMutex);
                pendingAssets.push({filename, std::move(buffer), PendingAsset::TEXTURE});
            }

            std::cout << "[AssetManager] Loaded texture data: " << filename << " (" << size << " bytes)" << std::endl;
        });
    }

    void loadFontAsync(const std::string& filename) {
        // Enqueue background task to load font file data
        loadingPool.enqueue([this, filename]() {
            std::string fullPath = "assets/fonts/" + filename;

            // Read file into memory (runs on background thread)
            std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                std::cerr << "[AssetManager] Failed to open font: " << fullPath << std::endl;
                return;
            }

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            if (!file.read(buffer.data(), size)) {
                std::cerr << "[AssetManager] Failed to read font: " << fullPath << std::endl;
                return;
            }

            // Add to pending queue (will be processed on main thread)
            {
                std::lock_guard<std::mutex> lock(pendingMutex);
                pendingAssets.push({filename, std::move(buffer), PendingAsset::FONT});
            }

            std::cout << "[AssetManager] Loaded font data: " << filename << " (" << size << " bytes)" << std::endl;
        });
    }

    void processPendingAssets() {
        // Process all pending assets loaded in background threads
        // This MUST run on the main thread (SFML OpenGL context requirement)

        std::lock_guard<std::mutex> lock(pendingMutex);

        while (!pendingAssets.empty()) {
            PendingAsset& pending = pendingAssets.front();

            if (pending.type == PendingAsset::TEXTURE) {
                // Create SFML texture from loaded data (main thread only!)
                auto texture = std::make_shared<sf::Texture>();
                if (texture->loadFromMemory(pending.fileData.data(), pending.fileData.size())) {
                    textureCache[pending.key] = texture;
                    textureOrder.push_back(pending.key);
                    std::cout << "[AssetManager] Finalized texture: " << pending.key << std::endl;
                } else {
                    std::cerr << "[AssetManager] Failed to create texture from data: " << pending.key << std::endl;
                }
            } else if (pending.type == PendingAsset::FONT) {
                // Create SFML font from loaded data (main thread only!)
                auto font = std::make_shared<sf::Font>();
                if (font->loadFromMemory(pending.fileData.data(), pending.fileData.size())) {
                    fontCache[pending.key] = font;
                    fontOrder.push_back(pending.key);
                    std::cout << "[AssetManager] Finalized font: " << pending.key << std::endl;
                } else {
                    std::cerr << "[AssetManager] Failed to create font from data: " << pending.key << std::endl;
                }
            }

            pendingAssets.pop();
        }
    }

public:
    // Singleton access
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    // Delete copy/move constructors (singleton)
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetManager(AssetManager&&) = delete;
    AssetManager& operator=(AssetManager&&) = delete;

    /**
     * Start loading all known assets in background
     * Call this once during game initialization
     */
    void preloadAllAssets() {
        // Preload all known assets used by the game
        // Assets are discovered from current codebase usage

        // Textures
        loadTextureAsync("tile000.png");

        // Fonts (only load once, shared by all entities)
        loadFontAsync("sansation.ttf");

        std::cout << "[AssetManager] Started background loading of assets..." << std::endl;
    }

    /**
     * Process pending assets loaded in background threads
     * MUST be called each frame from main thread to finalize SFML resources
     */
    void update() {
        // Process any assets that finished loading in background
        processPendingAssets();
    }

    /**
     * Get a texture by name (returns nullptr if not loaded yet)
     * @param name Filename relative to assets/images/ directory
     * @return Shared pointer to texture, or nullptr if not ready
     */
    std::shared_ptr<sf::Texture> getTexture(const std::string& name) {
        auto it = textureCache.find(name);
        if (it != textureCache.end()) {
            return it->second;
        }
        return nullptr;  // Not loaded yet
    }

    /**
     * Get a font by name (returns nullptr if not loaded yet)
     * @param name Filename relative to assets/fonts/ directory
     * @return Shared pointer to font, or nullptr if not ready
     */
    std::shared_ptr<sf::Font> getFont(const std::string& name) {
        auto it = fontCache.find(name);
        if (it != fontCache.end()) {
            return it->second;
        }
        return nullptr;  // Not loaded yet
    }

    /**
     * Check if a texture is loaded and ready
     */
    bool isTextureLoaded(const std::string& name) const {
        return textureCache.find(name) != textureCache.end();
    }

    /**
     * Check if a font is loaded and ready
     */
    bool isFontLoaded(const std::string& name) const {
        return fontCache.find(name) != fontCache.end();
    }

    /**
     * Get all texture names in insertion order
     * Useful for iterating through loaded textures
     */
    const std::vector<std::string>& getTextureNames() const { return textureOrder; }

    /**
     * Get all font names in insertion order
     * Useful for iterating through loaded fonts
     */
    const std::vector<std::string>& getFontNames() const { return fontOrder; }

    /**
     * Get loading statistics
     */
    size_t getLoadedTextureCount() const { return textureCache.size(); }
    size_t getLoadedFontCount() const { return fontCache.size(); }
    size_t getPendingAssetCount() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(pendingMutex));
        return pendingAssets.size();
    }
};

#endif // ASSET_MANAGER_HPP
