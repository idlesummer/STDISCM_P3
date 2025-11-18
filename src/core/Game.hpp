#pragma once
#include "Scene.hpp"
#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

using namespace sf;
using namespace std;


// Main game engine with scene management (like React Router!)
class Game {
private:
    RenderWindow window;
    shared_ptr<Scene> currentScene;

public:
    Game(int width, int height, const string& title)
        : window(VideoMode(width, height), title), currentScene(nullptr) {
        this->window.setFramerateLimit(165);
    }

    // Scene management (like React Router navigation)
    void changeScene(shared_ptr<Scene> newScene) {
        if (this->currentScene) {
            this->currentScene->onDestroy();        // Unmount old scene
            this->currentScene->clearEntities();    // Cleanup entities
        }

        this->currentScene = newScene;
        if (this->currentScene) {
            this->currentScene->setGame(this);      // Give scene access to game
            this->currentScene->onCreate();         // Mount new scene
        }
    }

    // Main game loop with fixed timestep
    void run() {
        auto TICK = seconds(1.f / 60.f);            // 60 updates per second
        auto clock = Clock();
        auto lag = Time::Zero;

        while (this->window.isOpen()) {
            auto elapsed = clock.restart();
            for (lag += elapsed; lag >= TICK; lag -= TICK) {
                this->handleEvents();               // 1. Process events and dispatch to current scene
                this->handleInputs(TICK);           // 2. Update current scene at fixed timestep
            }
            this->handleRender();                   // 3. Render phase runs as fast as possible (uncapped)
        }
        this->handleExit();                         // 4. Cleanup on exit
    }

    auto getCurrentScene() const { return this->currentScene; }

private:
    void handleInputs(Time TICK) {
        if (this->currentScene)
            this->currentScene->onUpdate(TICK);
    }

    void handleEvents() {
        auto event = Event();
        while (this->window.pollEvent(event)) {
            if (event.type == Event::Closed)
                this->window.close();
            if (this->currentScene)
                this->currentScene->onInput(event);
        }
    }

    void handleRender() {
        this->window.clear(Color::Black);
        if (this->currentScene)
            this->currentScene->onDraw(this->window);
        this->window.display();
    }

    void handleExit() {
        if (this->currentScene) {
            this->currentScene->onDestroy();
            this->currentScene->clearEntities();
        }
    }
};
