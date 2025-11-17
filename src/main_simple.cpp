#include <iostream>
#include <SFML/Graphics.hpp>

// Core engine
#include "core/ReactSFMLEngine.h"
#include "core/Component.h"
#include "core/RenderNode.h"
#include "core/Store.h"

using namespace ReactSFML;

// Simple state - just position
struct SimpleState : public State {
    sf::Vector2f position;

    SimpleState() : position(400, 300) {}

    State* clone() const override {
        return new SimpleState(*this);
    }
};

// Simple actions
inline Action createMoveAction(sf::Vector2f delta) {
    return Action{"MOVE", delta};
}

// Simple reducer
inline SimpleState simpleReducer(const SimpleState& state, const Action& action) {
    SimpleState newState = state;

    if (action.type == "MOVE") {
        auto delta = action.getPayload<sf::Vector2f>();
        newState.position = state.position + delta;

        // Clamp to screen
        newState.position.x = std::max(50.0f, std::min(750.0f, newState.position.x));
        newState.position.y = std::max(50.0f, std::min(550.0f, newState.position.y));

        std::cout << "New position: (" << newState.position.x << ", " << newState.position.y << ")" << std::endl;
    }

    return newState;
}

// Simple component - just a circle
class SimpleCircle : public Component {
public:
    SimpleCircle(Store<SimpleState>* store)
        : Component("SimpleCircle"), store(store) {}

    std::shared_ptr<RenderNode> render() override {
        const auto& state = store->getState();

        Props circleProps;
        circleProps["radius"] = 30.0f;
        circleProps["color"] = sf::Color::Green;
        circleProps["position"] = state.position;

        return Circle(circleProps);
    }

private:
    Store<SimpleState>* store;
};

// Event handler
void handleEvents(sf::Event& event, Store<SimpleState>& store) {
    if (event.type == sf::Event::KeyPressed) {
        sf::Vector2f delta(0, 0);
        float speed = 10.0f;

        switch (event.key.code) {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                delta.x = -speed;
                break;
            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                delta.x = speed;
                break;
            case sf::Keyboard::Up:
            case sf::Keyboard::W:
                delta.y = -speed;
                break;
            case sf::Keyboard::Down:
            case sf::Keyboard::S:
                delta.y = speed;
                break;
            default:
                return;
        }

        if (delta.x != 0 || delta.y != 0) {
            store.dispatch(createMoveAction(delta));
        }
    }
}

int main() {
    std::cout << "=== Simple React-SFML Demo ===" << std::endl;
    std::cout << "Use WASD or Arrow Keys to move the circle" << std::endl;
    std::cout << std::endl;

    // Create engine
    ReactSFMLEngine<SimpleState> engine(
        800, 600,
        "Simple React-SFML Demo",
        SimpleState(),
        simpleReducer
    );

    // Optional: Add logger to see actions
    engine.getStore().addMiddleware(createLoggerMiddleware());

    // Set event handler
    engine.setEventHandler(handleEvents);

    // Create simple circle component
    auto circle = std::make_shared<SimpleCircle>(&engine.getStore());
    circle->mount();
    engine.setRoot(circle);

    // Run
    std::cout << "Starting engine..." << std::endl;
    engine.run();

    std::cout << "Done!" << std::endl;
    return 0;
}
