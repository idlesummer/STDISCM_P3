#pragma once

#include "Component.h"
#include "Reconciler.h"
#include "Store.h"
#include "StateManager.h"
#include "EffectManager.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace ReactSFML {

// Main engine class that manages the React-like game loop
template<typename TState>
class ReactSFMLEngine {
public:
    ReactSFMLEngine(int width, int height, const std::string& title,
                   TState initialState, Reducer<TState> reducer)
        : window(sf::VideoMode(width, height), title),
          store(initialState, reducer),
          reconciler(&window),
          rootComponent(nullptr) {

        window.setFramerateLimit(60);
    }

    // Set the root component (like ReactDOM.render)
    void setRoot(std::shared_ptr<Component> root) {
        rootComponent = root;
        if (rootComponent) {
            rootComponent->mount();
        }
    }

    // Get store for dispatching actions
    Store<TState>& getStore() {
        return store;
    }

    // Main game loop
    void run() {
        sf::Clock clock;

        while (window.isOpen()) {
            sf::Time deltaTime = clock.restart();

            // 1. Process events
            processEvents();

            // 2. Update components
            if (rootComponent) {
                rootComponent->onUpdate(deltaTime);
            }

            // 3. Run effects
            EffectManager::getInstance().runEffects();

            // 4. Render phase: generate virtual DOM
            if (rootComponent) {
                auto renderTree = rootComponent->render();

                // 5. Reconciliation phase: diff and update SFML objects
                reconciler.reconcile(renderTree);

                // 6. Commit phase: render to screen
                reconciler.render();
            }
        }
    }

    // Process SFML events and dispatch actions
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Pass event to event handler if set
            if (eventHandler) {
                eventHandler(event, store);
            }
        }
    }

    // Set custom event handler
    void setEventHandler(std::function<void(sf::Event&, Store<TState>&)> handler) {
        eventHandler = handler;
    }

private:
    sf::RenderWindow window;
    Store<TState> store;
    Reconciler reconciler;
    std::shared_ptr<Component> rootComponent;
    std::function<void(sf::Event&, Store<TState>&)> eventHandler;
};

} // namespace ReactSFML
