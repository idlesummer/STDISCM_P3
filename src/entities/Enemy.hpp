#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;
#include <iostream>
#include <cmath>


// Enemy entity that moves in a circular pattern
class Enemy : public Entity {
public:
    Enemy(Vector2f initialPosition)
        : Entity("Enemy"), position(initialPosition), speed(3.0f) {}

    void onCreate() override {
        cout << "Enemy spawned!" << endl;

        sprite.setRadius(25.0f);
        sprite.setFillColor(Color::Red);
        sprite.setOrigin(25.0f, 25.0f);
        sprite.setPosition(position);
    }

    void onUpdate(Time dt) override {
        // Simple AI: move in a circular pattern
        elapsedTime += dt.asSeconds();
        position.x = 400 + 200 * cos(elapsedTime * speed * 0.1f);
        position.y = 300 + 200 * sin(elapsedTime * speed * 0.1f);

        sprite.setPosition(position);
    }

    void onDraw(RenderWindow& window) override {
        window.draw(sprite);
    }

    Vector2f getPosition() const { return position; }

private:
    Vector2f position;
    float speed;
    float elapsedTime = 0.0f;
    CircleShape sprite;
};
