#pragma once

#include "../core/Entity.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;


// Player entity - clean and simple!
class Player : public Entity {
public:
    Player(Vector2f initialPosition = Vector2f(400, 300))
        : Entity("Player"), position(initialPosition), speed(10.0f) {}

    // Lifecycle hook - called once when entity is created
    void onCreate() override {
        std::cout << "Player entity created!" << std::endl;

        // Initialize sprite
        sprite.setRadius(30.0f);
        sprite.setFillColor(Color::Green);
        sprite.setOrigin(30.0f, 30.0f);  // Center the origin
        sprite.setPosition(position);
    }

    // Update hook - game logic every frame (like useEffect(() => {}, []))
    void update(Time dt) override {
        bool moved = false;
        Vector2f delta(0, 0);

        // Handle input (like event handlers in React)
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

        // Update state (like this.setState() in React)
        if (moved) {
            position = position + delta;

            // Clamp to screen bounds
            position.x = std::max(50.0f, std::min(750.0f, position.x));
            position.y = std::max(50.0f, std::min(550.0f, position.y));

            // Update sprite position
            sprite.setPosition(position);
        }
    }

    // Draw hook - direct rendering (no virtual DOM!)
    void draw(RenderWindow& window) override {
        window.draw(sprite);
    }

    // Cleanup hook - called when entity is destroyed
    void onDestroy() override {
        std::cout << "Player entity destroyed!" << std::endl;
    }

private:
    // State
    Vector2f position;
    float speed;

    // Visual representation
    CircleShape sprite;
};
