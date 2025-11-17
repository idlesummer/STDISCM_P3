#pragma once

#include "Scene.hpp"
#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

using namespace sf;
using namespace std;


// Main game engine with scene management (like React Router!)
class Game {
public:
    Game(int width, int height, const string& title)
        : window(VideoMode(width, height), title), currentScene(nullptr) {
        this->window.setFramerateLimit(165);
    }

    // Scene management (like React Router navigation)
    void changeScene(shared_ptr<Scene> newScene) {
        if (this->currentScene) {
            this->currentScene->onDestroy();      // Unmount old scene
            this->currentScene->clearEntities();  // Cleanup entities
        }

        this->currentScene = newScene;

        if (this->currentScene) {
            this->currentScene->setGame(this);    // Give scene access to game
            this->currentScene->onCreate();       // Mount new scene
        }
    }

    shared_ptr<Scene> getCurrentScene() const {
        return this->currentScene;
    }

    // Main game loop with fixed timestep
    void run() {
        auto TICK = seconds(1.f / 60.f);  // 60 updates per second
        auto clock = Clock();
        auto lag = Time::Zero;

        while (this->window.isOpen()) {
            for (lag += clock.restart(); lag >= TICK; lag -= TICK) {  // Fixed timestep update loop

                // 1. Process events and dispatch to current scene
                auto event = Event();
                while (this->window.pollEvent(event)) {
                    if (event.type == Event::Closed) this->window.close();  // Handle window close event
                    if (this->currentScene) this->currentScene->onInput(event);   // Dispatch input events to current scene
                }

                // 2. Update current scene at fixed timestep
                if (this->currentScene) this->currentScene->onUpdate(TICK);
            }

            // 3. Render phase - runs as fast as possible (uncapped)
            this->window.clear(Color::Black);
            if (this->currentScene) this->currentScene->onDraw(this->window);
            this->window.display();
        }

        // Cleanup on exit
        if (this->currentScene) {
            this->currentScene->onDestroy();
            this->currentScene->clearEntities();
        }
    }

    // Access to window
    RenderWindow& getWindow() { return this->window; }

private:
    RenderWindow window;
    shared_ptr<Scene> currentScene;
};
