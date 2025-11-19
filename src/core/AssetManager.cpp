#include "AssetManager.hpp"
#include <fstream>
#include <iostream>

AssetManager::AssetManager() : loadingPool(4) {
    // Initialize thread pool with 4 worker threads for background loading
}

void AssetManager::preloadAllAssets() {
    // Preload all known assets used by the game
    // Assets are discovered from current codebase usage

    // Textures
    loadTextureAsync("tile000.png");

    // Fonts (only load once, shared by all entities)
    loadFontAsync("sansation.ttf");

    std::cout << "[AssetManager] Started background loading of assets..." << std::endl;
}

void AssetManager::loadTextureAsync(const std::string& filename) {
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

void AssetManager::loadFontAsync(const std::string& filename) {
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

void AssetManager::processPendingAssets() {
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

void AssetManager::update() {
    // Process any assets that finished loading in background
    processPendingAssets();
}

std::shared_ptr<sf::Texture> AssetManager::getTexture(const std::string& name) {
    auto it = textureCache.find(name);
    if (it != textureCache.end()) {
        return it->second;
    }
    return nullptr;  // Not loaded yet
}

std::shared_ptr<sf::Font> AssetManager::getFont(const std::string& name) {
    auto it = fontCache.find(name);
    if (it != fontCache.end()) {
        return it->second;
    }
    return nullptr;  // Not loaded yet
}

bool AssetManager::isTextureLoaded(const std::string& name) const {
    return textureCache.find(name) != textureCache.end();
}

bool AssetManager::isFontLoaded(const std::string& name) const {
    return fontCache.find(name) != fontCache.end();
}
