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
        window.setFramerateLimit(165);
    }

    // Scene management (like React Router navigation)
    void changeScene(shared_ptr<Scene> newScene) {
        if (currentScene) {
            currentScene->onDestroy();      // Unmount old scene
            currentScene->clearEntities();  // Cleanup entities
        }

        currentScene = newScene;

        if (currentScene) {
            currentScene->setGame(this);    // Give scene access to game
            currentScene->onCreate();       // Mount new scene
        }
    }

    shared_ptr<Scene> getCurrentScene() const {
        return currentScene;
    }

    // Main game loop with fixed timestep
    void run() {
        auto TICK = seconds(1.f / 60.f);  // 60 updates per second
        auto clock = Clock();
        auto lag = Time::Zero;

        while (window.isOpen()) {
            for (lag += clock.restart(); lag >= TICK; lag -= TICK) {  // Fixed timestep update loop
                
                // 1. Process events and dispatch to current scene
                auto event = Event();
                while (window.pollEvent(event)) {                       
                    if (event.type == Event::Closed) window.close();  // Handle window close event
                    if (currentScene) currentScene->onInput(event);   // Dispatch input events to current scene
                }

                // 2. Update current scene at fixed timestep
                if (currentScene)
                    currentScene->onUpdate(TICK);
            }

            // 3. Render phase - runs as fast as possible (uncapped)
            window.clear(Color::Black);

            if (currentScene)
                currentScene->onDraw(window);

            window.display();
        }

        // Cleanup on exit
        if (currentScene) {
            currentScene->onDestroy();
            currentScene->clearEntities();
        }
    }

    // Access to window
    RenderWindow& getWindow() { return window; }

private:
    RenderWindow window;
    shared_ptr<Scene> currentScene;
};
