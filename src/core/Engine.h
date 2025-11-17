#pragma once

#include "component/Component.h"
#include "renderer/Reconciler.h"
#include "store/Store.h"
#include <SFML/Graphics.hpp>
#include <memory>


// Main engine class that manages the React-like game loop
template<typename TState>
class Engine {
public:
    Engine(int width, int height, const std::string& title, Store<TState>& store)
        : window(sf::VideoMode(width, height), title),
          store(&store),
          reconciler(&this->window),
          rootComponent(nullptr) {

        this->window.setFramerateLimit(165);
    }

    // Set the root component (like ReactDOM.render)
    void setRoot(std::shared_ptr<Component> root) {
        this->rootComponent = root;
        if (this->rootComponent)
            this->rootComponent->mount();
    }

    // Get store for dispatching actions
    Store<TState>& getStore() {
        return *this->store;
    }

    // Main game loop
    void run() {
        auto clock = sf::Clock();

        while (this->window.isOpen()) {
            auto deltaTime = clock.restart();
            this->processEvents();                  // 1. Process events
            
            if (this->rootComponent)                // 2. Update components
                this->rootComponent->onUpdate(deltaTime);

            if (!this->rootComponent)               // 3. Render phase: generate virtual DOM
                continue;

            auto renderTree = this->rootComponent->render();
            this->reconciler.reconcile(renderTree); // 4. Diff and update SFML objects
            this->reconciler.render();              // 5. Commit phase: render to screen
        }
    }

    // Process SFML events and dispatch actions
    void processEvents() {
        auto event = sf::Event();
        while (this->window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                this->window.close();

            // Pass event to event handler if set
            if (this->eventHandler)
                this->eventHandler(event, *this->store);
        }
    }

    // Set custom event handler
    void setEventHandler(std::function<void(sf::Event&, Store<TState>&)> handler) {
        this->eventHandler = handler;
    }

private:
    sf::RenderWindow window;
    Store<TState>* store;
    Reconciler reconciler;
    std::shared_ptr<Component> rootComponent;
    std::function<void(sf::Event&, Store<TState>&)> eventHandler;
};
