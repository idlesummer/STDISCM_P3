#include <iostream>
#include <SFML/Graphics.hpp>

// Core engine with new structure
#include "core/Engine.h"
#include "core/component/Component.h"
#include "core/types/Props.h"

using namespace sf;
using namespace std;


// Simple component with internal state - React-style!
class CircleComponent : public Component {
public:
    CircleComponent(Vector2f initialPosition = Vector2f(400, 300))
        : Component("Circle"), position(initialPosition), speed(10.0f) {}

    // Lifecycle method - called once when component mounts
    void componentDidMount() override {
        cout << "Circle component mounted!" << endl;
    }

    // Lifecycle method - called when component unmounts
    void componentWillUnmount() override {
        cout << "Circle component unmounting..." << endl;
    }

    // Update hook - handle input and update state (like event handlers in React)
    void onUpdate(Time deltaTime) override {
        bool moved = false;
        Vector2f delta(0, 0);

        // Check keyboard state (like handling events in React)
        if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) {
            delta.x = -this->speed;
            moved = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) {
            delta.x = this->speed;
            moved = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) {
            delta.y = -this->speed;
            moved = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) {
            delta.y = this->speed;
            moved = true;
        }

        // Update internal state (like this.setState() in React)
        if (moved) {
            this->position = this->position + delta;

            // Clamp to screen bounds
            this->position.x = max(50.0f, min(750.0f, this->position.x));
            this->position.y = max(50.0f, min(550.0f, this->position.y));

            this->setState();  // Trigger re-render
        }
    }

    // Render method - returns virtual DOM (like React's render())
    shared_ptr<RenderNode> render() override {
        return Circle(Props{
            {"position", this->position},
            {"color", Color::Green},
            {"radius", 30.0f},
        });
    }

private:
    Vector2f position;  // Internal component state
    float speed;
};

// Main entry point
int main() {
    cout << "=== Reactive SFML - Class-Based React ===" << endl;
    cout << "Controls: WASD or Arrow Keys to move" << endl;
    cout << endl;

    // Create engine (like ReactDOM.render)
    auto engine = Engine(800, 600, "Reactive SFML - Moving Circle");

    // Create and mount the circle component with initial props
    auto circle = make_shared<CircleComponent>(Vector2f(400, 300));
    engine.setRoot(circle);

    // Run the game loop
    engine.run();

    cout << "Done!" << endl;
    return 0;
}
