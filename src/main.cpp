#include <iostream>
#include <SFML/Graphics.hpp>

// Core engine with new structure
#include "core/Engine.h"
#include "core/component/Component.h"
#include "core/types/Props.h"
#include "core/types/State.h"
#include "core/types/Action.h"
#include "core/store/Store.h"
#include "core/store/Middleware.h"

// Simple state - just position
struct GameState : public State {
    sf::Vector2f position;

    GameState() : position(400, 300) {}

    State* clone() const override {
        return new GameState(*this);
    }
};

// Action creators
inline Action createMoveAction(sf::Vector2f delta) {
    return Action{"MOVE", delta};
}

// Reducer - pure function that updates state
inline GameState gameReducer(const GameState& state, const Action& action) {
    auto newState = GameState(state);

    if (action.type == "MOVE") {
        auto delta = action.getPayload<sf::Vector2f>();
        newState.position = state.position + delta;

        // Clamp to screen bounds
        newState.position.x = std::max(50.0f, std::min(750.0f, newState.position.x));
        newState.position.y = std::max(50.0f, std::min(550.0f, newState.position.y));
    }

    return newState;
}

// Simple component - class-based React style (no hooks!)
class CircleComponent : public Component {
public:
    CircleComponent(Store<GameState>* store)
        : Component("Circle"), store(store) {}

    // Lifecycle method - called once when component mounts
    void componentDidMount() override {
        std::cout << "Circle component mounted!" << std::endl;
    }

    // Lifecycle method - called when component unmounts
    void componentWillUnmount() override {
        std::cout << "Circle component unmounting..." << std::endl;
    }

    std::shared_ptr<RenderNode> render() override {
        auto state = this->store->getState();

        auto props = Props();
        props["radius"] = 30.0f;
        props["color"] = sf::Color::Green;
        props["position"] = state.position;

        return Circle(props);
    }

private:
    Store<GameState>* store;
};

// Event handler - converts SFML events to actions
void handleEvents(sf::Event& event, Store<GameState>& store) {
    if (event.type == sf::Event::KeyPressed) {
        auto delta = sf::Vector2f(0, 0);
        auto speed = 10.0f;

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

        if (delta.x != 0 || delta.y != 0)
            store.dispatch(createMoveAction(delta));
    }
}

// Main entry point
int main() {
    std::cout << "=== Reactive SFML - Class-Based React ===" << std::endl;
    std::cout << "Controls: WASD or Arrow Keys to move" << std::endl;
    std::cout << std::endl;

    // Create store with initial state and reducer (like React's createStore)
    auto store = Store<GameState>(GameState(), gameReducer);

    // Optional: Add logger to see actions
    store.addMiddleware(createLoggerMiddleware());

    // Create engine with store (like React's ReactDOM.render with Provider)
    auto engine = Engine<GameState>(800, 600, "Reactive SFML - Moving Circle", store);

    // Set event handler
    engine.setEventHandler(handleEvents);

    // Create and mount the circle component (passing store reference)
    auto circle = std::make_shared<CircleComponent>(&store);
    engine.setRoot(circle);

    // Run the game loop
    engine.run();

    std::cout << "Done!" << std::endl;
    return 0;
}
