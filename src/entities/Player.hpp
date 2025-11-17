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
        : Entity("Player", initialPosition),
          speed(200.0f),
          moveLeft(false), moveRight(false), moveUp(false), moveDown(false) {}

    void onCreate() override {
        cout << "Player entity created!" << endl;

        // Initialize sprite
        this->sprite.setRadius(30.0f);
        this->sprite.setFillColor(Color::Green);
        this->sprite.setOrigin(30.0f, 30.0f);  // Center the origin
        this->sprite.setPosition(this->position);
    }

    // Input hook - track key press/release events (event-based, like React event handlers)
    void onInput(Event& event) override {
        auto isPressed = (event.type == Event::KeyPressed);
        auto isReleased = (event.type == Event::KeyReleased);

        if (isPressed || isReleased) {
            if (event.key.code == Keyboard::Left || event.key.code == Keyboard::A)
                this->moveLeft = isPressed;
            if (event.key.code == Keyboard::Right || event.key.code == Keyboard::D)
                this->moveRight = isPressed;
            if (event.key.code == Keyboard::Up || event.key.code == Keyboard::W)
                this->moveUp = isPressed;
            if (event.key.code == Keyboard::Down || event.key.code == Keyboard::S)
                this->moveDown = isPressed;
        }
    }

    // Update hook - apply movement based on tracked input state
    void onUpdate(Time dt) override {
        // Calculate velocity from tracked key states
        auto velocity = Vector2f(0, 0);

        if (this->moveLeft)
            velocity.x = -this->speed;
        if (this->moveRight)
            velocity.x = this->speed;
        if (this->moveUp)
            velocity.y = -this->speed;
        if (this->moveDown)
            velocity.y = this->speed;

        // Update position based on velocity and delta time
        this->position += velocity * dt.asSeconds();

        // Clamp to screen bounds
        this->position.x = max(50.0f, min(750.0f, this->position.x));
        this->position.y = max(50.0f, min(550.0f, this->position.y));

        // Update sprite position
        this->sprite.setPosition(this->position);
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->sprite);
    }

    void onDestroy() override {
        cout << "Player entity destroyed!" << endl;
    }

private:
    // Movement state
    float speed;  // pixels per second
    bool moveLeft, moveRight, moveUp, moveDown;

    // Visual representation
    CircleShape sprite;
};
