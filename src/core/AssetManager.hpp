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


class AssetManager {
public:
    AssetManager(size_t nthreads = 0)
        : pool(nthreads == 0 ? thread::hardware_concurrency() : nthreads),
          fonts(),
          fontsMutex(),
          loadingCount(0) {

        cout << "[AssetManager] Initialized with " << pool.getThreadCount() << " worker threads" << endl;
    }

    ~AssetManager() {
        cout << "[AssetManager] Shutting down..." << endl;
        this->awaitLoadingAssets();  // Ensure all loading completes before destruction
    }

    void loadFontAsync(const string& id, const string& path) {
        this->loadingCount++;

        this->pool.enqueue([this, id, path]() {
            auto font = make_shared<Font>();
            auto loaded = false;

            cout << "[AssetManager] Loading font: " << id << " from " << path << endl;

            // Try primary path
            if (font->loadFromFile(path)) {
                loaded = true;
                cout << "[AssetManager] Successfully loaded: " << id << " from " << path << endl;
            
            } else {
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

    auto getFont(const string& id) -> shared_ptr<Font> {
        auto lock = lock_guard<mutex>(this->fontsMutex);
        auto it = this->fonts.find(id);
        return it != this->fonts.end() ? it->second : nullptr;
    }

    auto isFontReady(const string& id) const {
        auto lock = lock_guard<mutex>(this->fontsMutex);
        return this->fonts.find(id) != this->fonts.end();
    }

    void awaitLoadingAssets() {
        cout << "[AssetManager] Waiting for all assets to load..." << endl;

        // Busy wait until all loading tasks complete and thread pool is idle
        while (!this->pool.isIdle() || this->loadingCount > 0) {
            this_thread::sleep_for(chrono::milliseconds(10));
        }

        cout << "[AssetManager] All assets loaded!" << endl;
    }

    bool allAssetsLoaded() const { return this->pool.isIdle() && this->loadingCount == 0; }
    auto getPendingCount() const -> int { return this->loadingCount; }

private:
    ThreadPool pool;
    map<string, shared_ptr<Font>> fonts;
    mutable mutex fontsMutex;
    atomic<int> loadingCount;
};
