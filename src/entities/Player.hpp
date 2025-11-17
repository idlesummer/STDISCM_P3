#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;


// Player entity - clean and simple!
class Player : public Entity {
public:
    Player(Vector2f initialPosition = Vector2f(400, 300))
        : Entity("Player"), position(initialPosition), speed(10.0f) {}

    // Lifecycle hook - called once when entity is created
    void onCreate() override {
        cout << "Player entity created!" << endl;

        // Initialize sprite
        sprite.setRadius(30.0f);
        sprite.setFillColor(Color::Green);
        sprite.setOrigin(30.0f, 30.0f);  // Center the origin
        sprite.setPosition(position);
    }

    // Input hook - handle input (like event handlers in React: onClick, onChange, etc.)
    void onInput() override {
        bool moved = false;
        Vector2f delta(0, 0);

        // Handle keyboard input
        if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) {
            delta.x = -speed;
            moved = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) {
            delta.x = speed;
            moved = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) {
            delta.y = -speed;
            moved = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) {
            delta.y = speed;
            moved = true;
        }

        // Update state based on input (like this.setState() in React)
        if (moved) {
            position = position + delta;

            // Clamp to screen bounds
            position.x = max(50.0f, min(750.0f, position.x));
            position.y = max(50.0f, min(550.0f, position.y));

            // Update sprite position
            sprite.setPosition(position);
        }
    }

    // Update hook - game logic every frame (like useEffect(() => {}, []))
    void onUpdate(Time dt) override {
        // Game logic that doesn't involve input goes here
        // For example: animation, physics, AI reactions, etc.
    }

    // Draw hook - direct rendering (no virtual DOM!)
    void onDraw(RenderWindow& window) override {
        window.draw(sprite);
    }

    // Cleanup hook - called when entity is destroyed
    void onDestroy() override {
        cout << "Player entity destroyed!" << endl;
    }

private:
    // State
    Vector2f position;
    float speed;

    // Visual representation
    CircleShape sprite;
};
