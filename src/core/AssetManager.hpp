#pragma once
#include "../utils/ThreadPool.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <iostream>

using namespace sf;
using namespace std;


/**
 * AssetManager - Multi-threaded asset loading system
 *
 * Loads fonts (and potentially other assets) asynchronously using a ThreadPool.
 * Provides thread-safe access to loaded assets.
 */
class AssetManager {
public:
    AssetManager(size_t numThreads = 4)
        : pool(numThreads),
          fonts(),
          fontsMutex(),
          loadingCount(0) {
        cout << "[AssetManager] Initialized with " << numThreads << " worker threads" << endl;
    }

    ~AssetManager() {
        cout << "[AssetManager] Shutting down..." << endl;
        this->waitForAll();  // Ensure all loading completes before destruction
    }

    /**
     * Load a font asynchronously from a file path.
     * Falls back to an alternative path if the primary path fails.
     *
     * @param id Unique identifier for this font
     * @param primaryPath Primary file path to load from
     * @param fallbackPath Optional fallback path if primary fails
     */
    void loadFontAsync(const string& id, const string& primaryPath, const string& fallbackPath = "") {
        this->loadingCount++;

        this->pool.enqueue([this, id, primaryPath, fallbackPath]() {
            cout << "[AssetManager] Loading font: " << id << " from " << primaryPath << endl;

            auto font = make_shared<Font>();
            bool loaded = false;

            // Try primary path
            if (font->loadFromFile(primaryPath)) {
                loaded = true;
                cout << "[AssetManager] Successfully loaded: " << id << " from " << primaryPath << endl;
            }
            // Try fallback path if provided
            else if (!fallbackPath.empty() && font->loadFromFile(fallbackPath)) {
                loaded = true;
                cout << "[AssetManager] Loaded " << id << " from fallback: " << fallbackPath << endl;
            }
            else {
                cout << "[AssetManager] WARNING: Failed to load font: " << id << endl;
            }

            // Store the font (even if loading failed, to mark it as "attempted")
            if (loaded) {
                auto lock = lock_guard<mutex>(this->fontsMutex);
                this->fonts[id] = font;
            }

            this->loadingCount--;
        });
    }

    /**
     * Get a loaded font by ID.
     * Returns nullptr if font is not yet loaded or failed to load.
     *
     * @param id Font identifier
     * @return Shared pointer to Font, or nullptr if not available
     */
    shared_ptr<Font> getFont(const string& id) {
        auto lock = lock_guard<mutex>(this->fontsMutex);
        auto it = this->fonts.find(id);
        if (it != this->fonts.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * Check if a font has finished loading (successfully or not)
     *
     * @param id Font identifier
     * @return true if font loading is complete
     */
    bool isReady(const string& id) const {
        auto lock = lock_guard<mutex>(this->fontsMutex);
        return this->fonts.find(id) != this->fonts.end();
    }

    /**
     * Wait for all pending asset loading tasks to complete
     */
    void waitForAll() {
        cout << "[AssetManager] Waiting for all assets to load..." << endl;

        // Busy wait until all loading tasks complete and thread pool is idle
        while (!this->pool.isIdle() || this->loadingCount > 0) {
            this_thread::sleep_for(chrono::milliseconds(10));
        }

        cout << "[AssetManager] All assets loaded!" << endl;
    }

    /**
     * Check if all assets have finished loading
     */
    bool allReady() const {
        return this->pool.isIdle() && this->loadingCount == 0;
    }

    /**
     * Get the number of assets currently being loaded
     */
    int getPendingCount() const {
        return this->loadingCount;
    }

private:
    ThreadPool pool;
    map<string, shared_ptr<Font>> fonts;
    mutable mutex fontsMutex;
    atomic<int> loadingCount;
};
