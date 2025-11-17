#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>
#include <string>


// Score display that updates in real-time
class ScoreText : public Entity {
public:
    ScoreText() : Entity("ScoreText"), score(0) {}

    void onCreate() override {
        if (!font.loadFromFile("assets/sansation.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition(10, 10);
        updateText();
    }

    void onUpdate(sf::Time dt) override {
        score += dt.asSeconds() * 10;  // Score increases over time
        updateText();
    }

    void onDraw(sf::RenderWindow& window) override {
        window.draw(text);
    }

    int getScore() const { return static_cast<int>(score); }

private:
    void updateText() {
        text.setString("Score: " + std::to_string(static_cast<int>(score)));
    }

    sf::Font font;
    sf::Text text;
    float score;
};
