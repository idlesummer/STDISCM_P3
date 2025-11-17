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

using namespace sf;
using namespace std;


// Simple state - just position
struct GameState : public State {
    Vector2f position;

    GameState() : position(400, 300) {}

    State* clone() const override {
        return new GameState(*this);
    }
};

// Reducer - pure function that updates state
GameState gameReducer(const GameState& state, const Action& action) {
    auto newState = GameState(state);

    if (action.type == "MOVE") {
        auto delta = action.getPayload<Vector2f>();
        newState.position = state.position + delta;

        // Clamp to screen bounds
        newState.position.x = max(50.0f, min(750.0f, newState.position.x));
        newState.position.y = max(50.0f, min(550.0f, newState.position.y));
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
        cout << "Circle component mounted!" << endl;
    }

    // Lifecycle method - called when component unmounts
    void componentWillUnmount() override {
        cout << "Circle component unmounting..." << endl;
    }

    shared_ptr<RenderNode> render() override {
        auto state = this->store->getState();

        auto props = Props();
        props["radius"] = 30.0f;
        props["color"] = Color::Green;
        props["position"] = state.position;

        return Circle(props);
    }

private:
    Store<GameState>* store;
};

// Event handler - converts SFML events to actions
void handleEvents(Event& event, Store<GameState>& store) {
    if (event.type == Event::KeyPressed) {
        auto delta = Vector2f(0, 0);
        auto speed = 10.0f;

        switch (event.key.code) {
            case Keyboard::Left:
            case Keyboard::A:
                delta.x = -speed;
                break;
            case Keyboard::Right:
            case Keyboard::D:
                delta.x = speed;
                break;
            case Keyboard::Up:
            case Keyboard::W:
                delta.y = -speed;
                break;
            case Keyboard::Down:
            case Keyboard::S:
                delta.y = speed;
                break;
            default:
                return;
        }

        if (delta.x != 0 || delta.y != 0)
            store.dispatch(Action{"MOVE", delta});
    }
}

// Main entry point
int main() {
    cout << "=== Reactive SFML - Class-Based React ===" << endl;
    cout << "Controls: WASD or Arrow Keys to move" << endl;
    cout << endl;

    // Create store with initial state and reducer (like React's createStore)
    auto store = Store<GameState>(GameState(), gameReducer);
    store.addMiddleware(createLoggerMiddleware());  // Optional: Add logger to see actions

    // Create engine with store (like React's ReactDOM.render with Provider)
    auto engine = Engine<GameState>(800, 600, "Reactive SFML - Moving Circle", store);
    engine.setEventHandler(handleEvents);   // Set event handler

    // Create and mount the circle component (passing store reference)
    auto circle = make_shared<CircleComponent>(&store);
    engine.setRoot(circle);

    // Run the game loop
    engine.run();

    cout << "Done!" << endl;
    return 0;
}
