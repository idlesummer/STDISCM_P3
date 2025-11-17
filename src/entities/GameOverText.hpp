#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <string>


// Game Over text display with customizable content, position, size, and color
class GameOverText : public Entity {
public:
    GameOverText(const std::string& content, sf::Vector2f position, int size, sf::Color color)
        : Entity("GameOverText") {
        this->content = content;
        this->position = position;
        this->size = size;
        this->color = color;
    }

    void onCreate() override {
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        text.setFont(font);
        text.setString(content);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setPosition(position);

        // Center the text
        auto bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2);
    }

    void onDraw(sf::RenderWindow& window) override {
        window.draw(text);
    }

private:
    sf::Font font;
    sf::Text text;
    std::string content;
    sf::Vector2f position;
    int size;
    sf::Color color;
};
