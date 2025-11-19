#pragma once
#include "Scene.hpp"
#include "Entity.hpp"
#include "AssetManager.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

using namespace sf;
using namespace std;


// Main game engine with scene management (like React Router!)
class Game {
private:
    RenderWindow window;
    shared_ptr<Scene> activeScene;

public:
    Game(int width, int height, const string& title)
        : window(VideoMode(width, height), title), activeScene(nullptr) {

        this->window.setFramerateLimit(165);

        // Start background loading of all game assets
        AssetManager::getInstance().preloadAllAssets();
    }

    // Scene management (like React Router navigation)
    void changeScene(shared_ptr<Scene> newScene) {
        if (this->activeScene) {
            this->activeScene->onDestroy();        // Unmount old scene
            this->activeScene->clearEntities();    // Cleanup entities
        }

        this->activeScene = newScene;
        if (this->activeScene) {
            this->activeScene->setGame(this);      // Give scene access to game
            this->activeScene->onCreate();         // Mount new scene
        }
    }

    // Main game loop with fixed timestep
    void run() {
        auto TICK = seconds(1.f / 60.f);            // 60 updates per second
        auto clock = Clock();
        auto lag = Time::Zero;

        while (this->window.isOpen()) {
            // Process pending assets loaded in background (non-blocking)
            AssetManager::getInstance().update();

            auto elapsed = clock.restart();
            for (lag += elapsed; lag >= TICK; lag -= TICK) {
                this->handleEvents();               // 1. Process events and dispatch to current scene
                this->handleInputs(TICK);           // 2. Update current scene at fixed timestep
            }
            this->handleRender();                   // 3. Render phase runs as fast as possible (uncapped)
        }
        this->handleExit();                         // 4. Cleanup on exit
    }

    auto getActiveScene() const {
        return this->activeScene;
    }

private:
    void handleInputs(Time TICK) {
        if (this->activeScene)
            this->activeScene->onUpdate(TICK);
    }

    void handleEvents() {
        auto event = Event();
        while (this->window.pollEvent(event)) {
            if (event.type == Event::Closed)
                this->window.close();
            if (this->activeScene)
                this->activeScene->onInput(event);
        }
    }

    void handleRender() {
        this->window.clear(Color::Black);
        if (this->activeScene)
            this->activeScene->onDraw(this->window);
        this->window.display();
    }

    void handleExit() {
        if (this->activeScene) {
            this->activeScene->onDestroy();
            this->activeScene->clearEntities();
        }
    }
};
