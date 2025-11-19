#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include "../utils/ThreadPool.hpp"

/**
 * AssetManager - Singleton asset loading and caching system
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
    AssetManager();

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
    void loadTextureAsync(const std::string& filename);
    void loadFontAsync(const std::string& filename);
    void processPendingAssets();  // Called from update() on main thread

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
    void preloadAllAssets();

    /**
     * Process pending assets loaded in background threads
     * MUST be called each frame from main thread to finalize SFML resources
     */
    void update();

    /**
     * Get a texture by name (returns nullptr if not loaded yet)
     * @param name Filename relative to assets/images/ directory
     * @return Shared pointer to texture, or nullptr if not ready
     */
    std::shared_ptr<sf::Texture> getTexture(const std::string& name);

    /**
     * Get a font by name (returns nullptr if not loaded yet)
     * @param name Filename relative to assets/fonts/ directory
     * @return Shared pointer to font, or nullptr if not ready
     */
    std::shared_ptr<sf::Font> getFont(const std::string& name);

    /**
     * Check if a texture is loaded and ready
     */
    bool isTextureLoaded(const std::string& name) const;

    /**
     * Check if a font is loaded and ready
     */
    bool isFontLoaded(const std::string& name) const;

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
