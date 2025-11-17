#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>


// Enemy entity that moves in a circular pattern
class Enemy : public Entity {
public:
    Enemy(sf::Vector2f initialPosition)
        : Entity("Enemy"), position(initialPosition), speed(3.0f) {}

    void onCreate() override {
        std::cout << "Enemy spawned!" << std::endl;

        sprite.setRadius(25.0f);
        sprite.setFillColor(sf::Color::Red);
        sprite.setOrigin(25.0f, 25.0f);
        sprite.setPosition(position);
    }

    void onUpdate(sf::Time dt) override {
        // Simple AI: move in a circular pattern
        elapsedTime += dt.asSeconds();
        position.x = 400 + 200 * std::cos(elapsedTime * speed * 0.1f);
        position.y = 300 + 200 * std::sin(elapsedTime * speed * 0.1f);

        sprite.setPosition(position);
    }

    void onDraw(sf::RenderWindow& window) override {
        window.draw(sprite);
    }

    sf::Vector2f getPosition() const { return position; }

private:
    sf::Vector2f position;
    float speed;
    float elapsedTime = 0.0f;
    sf::CircleShape sprite;
};
