#pragma once

#include "../core/Entity.hpp"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;
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
        text.setFillColor(Color::White);
        text.setPosition(10, 10);
        updateText();
    }

    void onUpdate(Time dt) override {
        score += dt.asSeconds() * 10;  // Score increases over time
        updateText();
    }

    void onDraw(RenderWindow& window) override {
        window.draw(text);
    }

    int getScore() const { return static_cast<int>(score); }

private:
    void updateText() {
        text.setString("Score: " + to_string(static_cast<int>(score)));
    }

    Font font;
    Text text;
    float score;
};
