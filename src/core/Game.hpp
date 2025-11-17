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

    // Main game loop
    void run() {
        Clock clock;

        while (window.isOpen()) {
            auto dt = clock.restart();

            // 1. Process events and dispatch to current scene
            Event event;
            while (window.pollEvent(event)) {
                // Handle window close event
                if (event.type == Event::Closed)
                    window.close();

                // Dispatch input events to current scene
                if (currentScene)
                    currentScene->onInput(event);
            }

            // 2. Update current scene
            if (currentScene)
                currentScene->onUpdate(dt);

            // 3. Render phase - clear, draw, display
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
