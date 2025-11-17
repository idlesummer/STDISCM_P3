#pragma once

#include "Component.h"
#include "Reconciler.h"
#include "Store.h"
#include "StateManager.h"
#include "EffectManager.h"
#include <SFML/Graphics.hpp>
#include <memory>


// Main engine class that manages the React-like game loop
template<typename TState>
class Engine {
public:
    Engine(int width, int height, const std::string& title,
           TState initialState, Reducer<TState> reducer)
        : window(sf::VideoMode(width, height), title),
          store(initialState, reducer),
          reconciler(&this->window),
          rootComponent(nullptr) {

        this->window.setFramerateLimit(60);
    }

    // Set the root component (like ReactDOM.render)
    void setRoot(std::shared_ptr<Component> root) {
        this->rootComponent = root;
        if (this->rootComponent)
            this->rootComponent->mount();
    }

    // Get store for dispatching actions
    Store<TState>& getStore() {
        return this->store;
    }

    // Main game loop
    void run() {
        auto clock = sf::Clock();

        while (this->window.isOpen()) {
            auto deltaTime = clock.restart();

            // 1. Process events
            this->processEvents();

            // 2. Update components
            if (this->rootComponent)
                this->rootComponent->onUpdate(deltaTime);

            // 3. Run effects
            EffectManager::getInstance().runEffects();

            // 4. Render phase: generate virtual DOM
            if (!this->rootComponent)
                continue;

            auto renderTree = this->rootComponent->render();

            // 5. Reconciliation phase: diff and update SFML objects
            this->reconciler.reconcile(renderTree);

            // 6. Commit phase: render to screen
            this->reconciler.render();
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
                this->eventHandler(event, this->store);
        }
    }

    // Set custom event handler
    void setEventHandler(std::function<void(sf::Event&, Store<TState>&)> handler) {
        this->eventHandler = handler;
    }

private:
    sf::RenderWindow window;
    Store<TState> store;
    Reconciler reconciler;
    std::shared_ptr<Component> rootComponent;
    std::function<void(sf::Event&, Store<TState>&)> eventHandler;
};

