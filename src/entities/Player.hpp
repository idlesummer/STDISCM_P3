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
        : Entity("Player", initialPosition), speed(200.0f) {}

    // Lifecycle hook - called once when entity is created
    void onCreate() override {
        cout << "Player entity created!" << endl;

        // Initialize sprite
        this->sprite.setRadius(30.0f);
        this->sprite.setFillColor(Color::Green);
        this->sprite.setOrigin(30.0f, 30.0f);  // Center the origin
        this->sprite.setPosition(this->position);
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
            velocity.x = -this->speed;
        if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
            velocity.x = this->speed;
        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
            velocity.y = -this->speed;
        if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
            velocity.y = this->speed;

        // Update position based on velocity and delta time
        this->position += velocity * dt.asSeconds();

        // Clamp to screen bounds
        this->position.x = max(50.0f, min(750.0f, this->position.x));
        this->position.y = max(50.0f, min(550.0f, this->position.y));

        // Update sprite position
        this->sprite.setPosition(this->position);
    }

    // Draw hook - direct rendering (no virtual DOM!)
    void onDraw(RenderWindow& window) override {
        window.draw(this->sprite);
    }

    // Cleanup hook - called when entity is destroyed
    void onDestroy() override {
        cout << "Player entity destroyed!" << endl;
    }

private:
    // State
    float speed;  // pixels per second

    // Visual representation
    CircleShape sprite;
};
