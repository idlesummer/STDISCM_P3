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
        : Entity("Player"), position(initialPosition), speed(200.0f) {}

    // Lifecycle hook - called once when entity is created
    void onCreate() override {
        cout << "Player entity created!" << endl;

        // Initialize sprite
        sprite.setRadius(30.0f);
        sprite.setFillColor(Color::Green);
        sprite.setOrigin(30.0f, 30.0f);  // Center the origin
        sprite.setPosition(position);
    }

    // Input hook - handle discrete input events (like onClick, onKeyPress)
    void onInput(Event& event) override {
        // Handle one-time input events here
        // For example: jumping when space is pressed, shooting, etc.
        //
        // if (event.type == Event::KeyPressed) {
        //     if (event.key.code == Keyboard::Space)
        //         jump();
        // }
    }

    // Update hook - continuous game logic (like useEffect(() => {}, []))
    void onUpdate(Time dt) override {
        // Handle continuous input (state-based)
        auto velocity = Vector2f(0, 0);

        if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
            velocity.x = -speed;
        if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
            velocity.x = speed;
        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
            velocity.y = -speed;
        if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
            velocity.y = speed;

        // Update position based on velocity and delta time
        position += velocity * dt.asSeconds();

        // Clamp to screen bounds
        position.x = max(50.0f, min(750.0f, position.x));
        position.y = max(50.0f, min(550.0f, position.y));

        // Update sprite position
        sprite.setPosition(position);
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
    float speed;  // pixels per second

    // Visual representation
    CircleShape sprite;
};
