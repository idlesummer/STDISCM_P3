#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>


// Simple text entity for menu displays
class MenuText : public Entity {
public:
    MenuText(const std::string& content, sf::Vector2f position, int size = 48)
        : Entity("MenuText") {
        text.setString(content);
        text.setCharacterSize(size);
        text.setFillColor(sf::Color::White);
        text.setPosition(position);
    }

    void onCreate() override {
        // Try to load font (will use default if fails)
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        text.setFont(font);
    }

    void onDraw(sf::RenderWindow& window) override {
        window.draw(text);
    }

private:
    sf::Font font;
    sf::Text text;
};
