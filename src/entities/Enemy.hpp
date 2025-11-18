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
        : Entity("Enemy", initialPosition), speed(3.0f) {}

    void onCreate() override {
        cout << "Enemy spawned!" << endl;

        this->sprite.setRadius(25.0f);
        this->sprite.setFillColor(Color::Red);
        this->sprite.setOrigin(25.0f, 25.0f);
        this->sprite.setPosition(this->position);
    }

    void onUpdate(Time dt) override {
        // Simple AI: move in a circular pattern
        this->elapsedTime += dt.asSeconds();
        this->position.x = 400 + 200 * cos(this->elapsedTime * this->speed * 0.1f);
        this->position.y = 300 + 200 * sin(this->elapsedTime * this->speed * 0.1f);
        this->sprite.setPosition(this->position);
    }

    void onDraw(RenderWindow& window) override {
        window.draw(this->sprite);
    }

private:
    float speed;
    float elapsedTime = 0.0f;
    CircleShape sprite;
};
